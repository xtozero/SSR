#include "Archive.h"
#include "AssetBuilder.h"
#include "Core/IEngine.h"
#include "Djb2Hash.h"
#include "EngineDefaultBuilder.h"
#include "LibraryTool/Common.h"
#include "AssetBuilderConfig.h"
#include "Platform/CommandLine.h"
#include "Platform/IPlatform.h"
#include "Renderer/IRenderCore.h"
#include "SizedTypes.h"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <set>
#include <thread>

namespace fs = std::filesystem;

HMODULE g_engineDll = nullptr;

namespace
{
	class Console : public engine::IPlatform
	{
	public:
		virtual std::pair<uint32, uint32> GetSize() const noexcept override
		{
			return { 0, 0 };
		}

		virtual void UpdateSize( [[maybe_unused]] uint32 width, [[maybe_unused]] uint32 height ) override {}
		virtual void Resize( [[maybe_unused]] uint32 width, [[maybe_unused]] uint32 height ) override {}

	private:
		virtual void* GetRawHandleImple() const noexcept override
		{
			return nullptr;
		}
	};
	
	bool LoadModules()
	{
		fs::path oldPath = fs::current_path();
		fs::current_path( "../Program" );

		g_engineDll = LoadModule( "Engine.dll" );
		if ( g_engineDll == nullptr )
		{
			return false;
		}

		fs::current_path( oldPath );

		return true;
	}

	bool BootUpEngine( int32 argc, char* argv[] )
	{
		bool result = false;
		if ( auto engine = GetInterface<engine::IEngine>() )
		{
			Console console;

			std::string commandline;
			commandline.reserve( 2048 );

			for ( int32 i = 0; i < argc; ++i )
			{
				commandline += argv[i];
				commandline += " ";
			}
			commandline += "Console AssetBuilder";

			fs::path oldPath = fs::current_path();
			fs::current_path( "../Program" );

			result = engine->BootUp( console, commandline.c_str() );

			fs::current_path( oldPath );
		}

		return result;
	}

	void ReloadGlobalShaders()
	{
		fs::path oldPath = fs::current_path();
		fs::current_path( "../Program" );

		GetInterface<rendercore::IRenderCore>()->ReloadGlobalShaders();
		while ( GetInterface<rendercore::IRenderCore>()->IsReady() == false )
		{
			GetInterface<ITaskScheduler>()->ProcessThisThreadTask();
		}

		fs::current_path( oldPath );
	}

	bool IsIgnorePath( const PathEnvironment& env, const fs::path& path )
	{
		fs::path preferred = path;
		preferred.make_preferred();

		std::string pathStr = preferred.generic_string();

		for ( const fs::path& ignorePath : env.m_ignorePath )
		{
			if ( pathStr.starts_with( ignorePath.generic_string() ) )
			{
				return true;
			}
		}

		return false;
	}
	
	std::map<fs::path, AssetHeader> GatherAssetInfos()
	{
		std::map<fs::path, AssetHeader> assetInfos;
		std::set<fs::path> visited;

		for ( auto& [key, environment] : AssetBuilderConfig::Instance().PathEnvironments() )
		{
			if ( fs::exists( environment.m_destination ) )
			{
				for ( const auto& p : fs::recursive_directory_iterator( environment.m_destination ) )
				{
					if ( p.is_regular_file() )
					{
						if ( visited.contains( p ) )
						{
							continue;
						}

						std::ifstream asset( p.path(), std::ios::binary | std::ios::ate );
						std::streampos fileLen = asset.tellg();
						asset.seekg( 0, std::ios::beg );

						std::vector<char> buf( fileLen );

						asset.read( buf.data(), buf.size() );
						Archive ar( buf.data(), buf.size() );
						
						AssetHeader header;
						ar << header;

						assetInfos.emplace( p, header );
						visited.emplace( p );
					}
				}
			}
		}

		return assetInfos;
	}

	void RemoveUnusedAssets( const std::set<fs::path>& processed )
	{
		for ( auto& [key, environment] : AssetBuilderConfig::Instance().PathEnvironments() )
		{
			if ( fs::exists( environment.m_destination ) )
			{
				for ( const auto& p : fs::recursive_directory_iterator( environment.m_destination ) )
				{
					if ( p.is_regular_file() == false )
					{
						continue;
					}
					else if ( p.path().filename() == "ShaderCache.asset" )
					{
						continue;
					}

					if ( processed.contains( fs::absolute( p ) ) == false )
					{
						uint32 trial = 0;
						constexpr uint32 MaxTrial = 5;
						while ( trial < MaxTrial )
						{
							try
							{
								fs::remove( p );
								std::cout << "Unused asset (" + p.path().generic_string() + ") is removed" << std::endl;
								break;
							}
							catch ( fs::filesystem_error& )
							{
								++trial;
								if ( trial < MaxTrial )
								{
									std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
								}
								else
								{
									std::cerr << "Failed to remove unused asset (" + p.path().generic_string() + ")" << std::endl;
									break;
								}
							}
						}
					}
				}
			}
		}
	}

	uint64 CalcFileHash( const fs::path& path )
	{
		std::ifstream file( path, std::ios::binary );

		char buffer[1024] = {};
		uint64 hash = Djb2DefaultSeed;

		while ( file )
		{
			file.read( buffer, std::extent_v<decltype( buffer )> );
			size_t bytesRead = file.gcount();
			if ( bytesRead > 0 )
			{
				hash = Djb2Hash( buffer, bytesRead, hash );
			}
		}

		return hash;
	}

	void WriteToDisk( const std::unique_ptr<Serializable>& asset, const fs::path& path )
	{
		Archive archive;
		asset->Serialize( archive );
		archive.WriteToFile( path );

		if ( auto ayncLoadableAsset = Cast<AsyncLoadableAsset>( asset.get() ) )
		{
			fs::last_write_time( path, ayncLoadableAsset->LastWriteTime() );
		}
	}
}

int32 main( int32 argc, char* argv[] )
{
	fs::path workingPath = fs::current_path();
	AssetBuilderConfig::Instance().Load();

	if ( ( LoadModules() == false ) || ( BootUpEngine( argc, argv ) == false ) )
	{
		return EXIT_FAILURE;
	}

	std::map<fs::path, AssetHeader> assetInfos = GatherAssetInfos();

	AssetBuilder assetBuilder;
	assetBuilder.Initialize();

	struct AssetBuildSummary
	{
		uint32 m_succeededCount = 0;
		uint32 m_failedCount = 0;
		uint32 m_skippedCount = 0;
	} buildSummary;
	
	for ( auto& [key, environment] : AssetBuilderConfig::Instance().PreprocessingEnvironments() )
	{
		fs::path absolutePath = fs::absolute( environment.m_source );
		fs::recursive_directory_iterator iter( absolutePath );

		for ( const auto& p : iter )
		{
			if ( ( p.is_regular_file() == false ) 
				|| ( AssetBuilderConfig::Instance().IsPreprocessingAsset( p ) == false ) )
			{
				continue;
			}

			fs::path targetDirectory = environment.m_destination / fs::relative( p.path().parent_path() );

			auto products = assetBuilder.Build( environment, p.path(), CalcFileHash( p.path() ) );
			if ( products )
			{
				for ( const auto& product : products.value() )
				{
					if ( fs::exists( targetDirectory ) == false )
					{
						fs::create_directories( targetDirectory );
					}

					fs::path target = targetDirectory / product.first;
					target = fs::absolute( target.replace_extension( environment.m_productExtension ) );
					if ( ( environment.m_allowOverwrite == false )
						&& fs::exists( target ) )
					{
						continue;
					}

					const auto& asset = product.second;
					WriteToDisk( asset, target );

					++buildSummary.m_succeededCount;
				}
			}
			else
			{
				std::cerr << "Failed to process asset (" + p.path().generic_string() + ")" << std::endl;

				++buildSummary.m_failedCount;
			}
		}

		fs::current_path( workingPath );
	}

	std::set<fs::path> processed;
	for ( auto& [key, environment] : AssetBuilderConfig::Instance().PathEnvironments() )
	{
		fs::path absolutePath = fs::absolute( environment.m_source );
		fs::recursive_directory_iterator iter( absolutePath );

		fs::current_path( environment.m_source );

		for ( const auto& p : iter )
		{
			if ( IsIgnorePath( environment, p ) )
			{
				continue;
			}

			if ( p.is_regular_file() )
			{
				if ( AssetBuilderConfig::Instance().IsPreprocessingAsset( p ) )
				{
					continue;
				}

				fs::file_time_type lastWriteTime = fs::last_write_time( p );
				uint64 fileHash = CalcFileHash( p.path() );

				fs::path targetDirectory = environment.m_destination / fs::relative( p.path().parent_path() );
				fs::path target = targetDirectory / p.path().filename();
				target = fs::absolute( target.replace_extension( ".asset" ) );

				auto assetInfoIter = assetInfos.find( target );
				if ( assetInfoIter != std::end( assetInfos ) )
				{
					const AssetHeader& header = assetInfoIter->second;
					if ( ( header.m_lastWriteTime == lastWriteTime ) || ( header.m_fileHash == fileHash ) )
					{
						if ( engine::CommandLine::Has( StaticName( "LogSkippedAssets" ) ) )
						{
							std::cout << "Skip processing asset (" + p.path().generic_string() + ")" << std::endl;
						}

						processed.emplace( target );

						++buildSummary.m_skippedCount;
						continue;
					}
				}

				auto products = assetBuilder.Build( environment, p.path(), fileHash );
				if ( products )
				{
					for ( const auto& product : products.value() )
					{
						if ( fs::exists( targetDirectory ) == false )
						{
							fs::create_directories( targetDirectory );
						}

						const auto& asset = product.second;
						fs::path assetPath = targetDirectory / product.first;
						assetPath = fs::absolute( assetPath.replace_extension( ".asset" ) );

						WriteToDisk( asset, assetPath );

						processed.emplace( assetPath );

						++buildSummary.m_succeededCount;
					}
				}
				else
				{
					std::cerr << "Failed to process asset (" + p.path().generic_string() + ")" << std::endl;

					++buildSummary.m_failedCount;
				}
			}
		}

		fs::current_path( workingPath );
	}

	ReloadGlobalShaders();

	EngineDefaultBuilder engineDefault;
	engineDefault.Build( processed );

	ShutdownModule( g_engineDll );

	RemoveUnusedAssets( processed );

	return ( buildSummary.m_failedCount == 0 ) ? EXIT_SUCCESS : EXIT_FAILURE;
}