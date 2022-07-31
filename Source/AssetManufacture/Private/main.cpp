#include "Archive.h"
#include "AssetManufacturer.h"
#include "common.h"
#include "ManufactureConfig.h"
#include "SizedTypes.h"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <thread>

namespace fs = std::filesystem;

bool LoadModules()
{
	HMODULE engineDll = LoadModule( "Engine.dll" );
	if ( engineDll == nullptr )
	{
		return false;
	}

	HMODULE logicDll = LoadModule( "Logic.dll" );
	if ( logicDll == nullptr )
	{
		return false;
	}

	HMODULE renderCoreDll = LoadModule( "RenderCore.dll" );
	if ( renderCoreDll == nullptr )
	{
		return false;
	}

	HMODULE agaDll = LoadModule( "Aga.dll" );
	if ( agaDll == nullptr )
	{
		return false;
	}

	return true;
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
	for ( auto& [key, environment] : ManufactureConfig::Instance().Environments() )
	{
		uint32 trial = 0;
		constexpr uint32 maxTrial = 5;
		while ( trial < maxTrial )
		{
			if ( fs::exists( environment.m_destination ) == false )
			{
				break;
			}

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

int32 main()
{
	if ( LoadModules() == false )
	{
		return EXIT_FAILURE;
	}

	ManufactureConfig::Instance().Load();

	if ( PrepareDestinationDirectories() == false )
	{
		return EXIT_FAILURE;
	}

	AssetManufacturer manufacturer;

	fs::path rootPath = fs::current_path();

	for ( auto& [key, environment] : ManufactureConfig::Instance().Environments() )
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
				auto products = manufacturer.Manufacture( environment, p.path() );
				if ( products )
				{
					for ( const auto& product : products.value() )
					{
						const auto& fileName = product.first;
						const auto& archive = product.second;

						fs::path targetDirectory = environment.m_destination / fs::relative( p.path().parent_path() );
						fs::path target = targetDirectory / fileName;
						target.replace_extension( ".asset" );

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

	return EXIT_SUCCESS;
}