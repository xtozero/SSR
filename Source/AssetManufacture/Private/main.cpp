#include "Archive.h"
#include "ArchiveUtility.h"
#include "AssetManufacturer.h"
#include "common.h"
#include "Core/IEngine.h"
#include "EngineDefaultManufacturer.h"
#include "ManufactureConfig.h"
#include "Platform/IPlatform.h"
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
		commandline += "Console";

		fs::path oldPath = fs::current_path();
		fs::current_path( "../Program" );

		result = engine->BootUp( console, commandline.c_str() );

		fs::current_path( oldPath );
	}

	return result;
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

bool PrepareDestinationDirectories()
{
	for ( auto& [key, environment] : ManufactureConfig::Instance().PathEnvironments() )
	{
		if ( fs::exists( environment.m_destination ) )
		{
			uint32 trial = 0;
			constexpr uint32 maxTrial = 5;
			while ( trial < maxTrial )
			{
				if ( fs::is_directory( environment.m_destination ) )
				{
					try
					{
						fs::remove_all( environment.m_destination );
						break;
					}
					catch ( fs::filesystem_error& err )
					{
						++trial;
						std::cout << err.what() << " ( " << trial << " / " << maxTrial << " )\n";

						if ( trial < maxTrial )
						{
							std::cout << "Retry after 1 second\n";
							std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
						}
						else
						{
							return false;
						}
					}
				}
			}
		}

		if ( environment.m_duplicateSourceDirectoryStructure )
		{
			for ( const auto& p : fs::recursive_directory_iterator( environment.m_source ) )
			{
				if ( IsIgnorePath( environment, p ) )
				{
					continue;
				}

				if ( p.is_directory() )
				{
					fs::path rel = fs::relative( p.path(), environment.m_source );
					fs::create_directories( environment.m_destination / rel );
				}
			}
		}
		else
		{
			fs::create_directories( environment.m_destination );
		}
	}

	return true;
}

std::map<fs::path, fs::file_time_type> GatherAssetInfos()
{
	std::map<fs::path, fs::file_time_type> assetInfos;
	std::set<fs::path> visited;

	for ( auto& [key, environment] : ManufactureConfig::Instance().PathEnvironments() )
	{
		if ( fs::exists( environment.m_destination ) )
		{
			for ( const auto& p : fs::recursive_directory_iterator( environment.m_destination ) )
			{
				if ( p.is_regular_file() )
				{
					if ( visited.find( p ) != std::end( visited ) )
					{
						continue;
					}

					std::ifstream asset( p.path(), std::ios::binary | std::ios::ate );
					std::streampos fileLen = asset.tellg();
					asset.seekg( 0, std::ios::beg );

					std::vector<char> buf( fileLen );
					
					asset.read( buf.data(), buf.size() );
					Archive ar( buf.data(), buf.size() );

					uint32 id = 0;
					ar << id;

					fs::file_time_type lastWriteTime;
					ar << lastWriteTime;

					assetInfos.emplace( p, lastWriteTime);
					visited.emplace( p );
				}
			}
		}
	}

	return assetInfos;
}

void RemoveUnusedAssets( const std::set<fs::path>& processed )
{
	for ( auto& [key, environment] : ManufactureConfig::Instance().PathEnvironments() )
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

				if ( processed.find( fs::absolute( p ) ) == std::end( processed ) )
				{
					uint32 trial = 0;
					constexpr uint32 maxTrial = 5;
					while ( trial < maxTrial )
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
							if ( trial < maxTrial )
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

int32 main( int32 argc, char* argv[] )
{
	fs::path rootPath = fs::current_path();
	ManufactureConfig::Instance().Load();

	if ( LoadModules() == false )
	{
		return EXIT_FAILURE;
	}

	std::map<fs::path, fs::file_time_type> assetInfos = GatherAssetInfos();

	AssetManufacturer manufacturer;
	
	for ( auto& [key, environment] : ManufactureConfig::Instance().PreprocessingEnvironments() )
	{
		fs::path absolutePath = fs::absolute( environment.m_source );
		fs::recursive_directory_iterator iter( absolutePath );

		for ( const auto& p : iter )
		{
			if ( ( p.is_regular_file() == false ) 
				|| ( ManufactureConfig::Instance().IsPreprocessingAsset( p ) == false ) )
			{
				continue;
			}

			fs::path targetDirectory = environment.m_destination / fs::relative( p.path().parent_path() );

			auto products = manufacturer.Manufacture( environment, p.path() );
			if ( products )
			{
				for ( const auto& product : products.value() )
				{
					const auto& archive = product.second;

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

					archive.WriteToFile( target );
				}
			}
			else
			{
				std::cerr << "Failed to process asset (" + p.path().generic_string() + ")" << std::endl;
			}
		}

		fs::current_path( rootPath );
	}

	std::set<fs::path> processed;
	for ( auto& [key, environment] : ManufactureConfig::Instance().PathEnvironments() )
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
				if ( ManufactureConfig::Instance().IsPreprocessingAsset( p ) )
				{
					continue;
				}

				fs::file_time_type lastWriteTime = fs::last_write_time( p );

				fs::path targetDirectory = environment.m_destination / fs::relative( p.path().parent_path() );
				fs::path target = targetDirectory / p.path().filename();
				target = fs::absolute( target.replace_extension( ".asset" ) );

				processed.emplace( target );

				auto assetInfoIter = assetInfos.find( target );
				if ( assetInfoIter != std::end( assetInfos ) )
				{
					if ( assetInfoIter->second == lastWriteTime )
					{
						std::cout << "Skip processing asset (" + p.path().generic_string() + ")" << std::endl;
						continue;
					}
				}

				auto products = manufacturer.Manufacture( environment, p.path() );
				if ( products )
				{
					for ( const auto& product : products.value() )
					{
						const auto& archive = product.second;

						if ( fs::exists( targetDirectory ) == false )
						{
							fs::create_directories( targetDirectory );
						}

						archive.WriteToFile( target );
					}
				}
				else
				{
					std::cerr << "Failed to process asset (" + p.path().generic_string() + ")" << std::endl;
				}
			}
		}

		fs::current_path( rootPath );
	}

	if ( BootUpEngine( argc, argv ) == false )
	{
		return EXIT_FAILURE;
	}
	else
	{
		EngineDefaultManufacturer engineDefault;
		engineDefault.Manufacture( processed );
	}

	RemoveUnusedAssets( processed );

	ShutdownModule( g_engineDll );

	return EXIT_SUCCESS;
}