#include "Archive.h"
#include "AssetManufacturer.h"
#include "common.h"
#include "PathEnvironment.h"
#include "SizedTypes.h"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <thread>

namespace fs = std::filesystem;

bool LoadModules( )
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

bool PrepareDestinationDirectories( const fs::path& destRoot, const fs::path& srcRoot )
{
	uint32 trial = 0;
	constexpr uint32 maxTrial = 5;
	while ( trial < maxTrial )
	{
		if ( fs::exists( destRoot ) && fs::is_directory( destRoot ) )
		{
			try
			{
				fs::remove_all( destRoot );
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

	for ( const auto& p : fs::recursive_directory_iterator( srcRoot ) )
	{
		if ( p.is_directory( ) )
		{
			fs::path rel = fs::relative( p.path( ), srcRoot );
			fs::create_directories( destRoot / rel );
		}
	}

	return true;
}

int32 main( int32 argc, const char* argv[] )
{
	if ( argc < 3 )
	{
		return EXIT_FAILURE;
	}

	fs::path destDirectory = argv[1];
	fs::path srcDirectory = argv[2];

	PathEnvironment::Instance( ).Initialize( srcDirectory, destDirectory );

	if ( LoadModules( ) == false )
	{
		return EXIT_FAILURE;
	}

	if ( fs::is_directory( srcDirectory ) == false )
	{
		return EXIT_FAILURE;
	}

	if ( PrepareDestinationDirectories( destDirectory, srcDirectory ) == false )
	{
		return EXIT_FAILURE;
	}

	fs::current_path( srcDirectory );

	AssetManufacturer manufacturer;

	for ( const auto& p : fs::recursive_directory_iterator( srcDirectory ) )
	{
		if ( p.is_regular_file( ) )
		{
			fs::path targetDirectory = destDirectory / fs::relative( p.path( ).parent_path( ) );
			auto products = manufacturer.Manufacture( p.path( ), destDirectory );
			if ( products )
			{
				for ( const auto& product : products.value( ) )
				{
					const auto& fileName = product.first;
					const auto& archive = product.second;

					fs::path target = targetDirectory / fileName;
					target.replace_extension( ".asset" );

					archive.WriteToFile( target );
				}
			}
			else
			{
				std::cerr << "Failed to process asset (" + p.path( ).generic_string( ) + ")" << std::endl;
			}
		}
	}

	return EXIT_SUCCESS;
}