#include "AssetManufacturer.h"

#include "DDSManufacturer.h"
#include "JsonManufacturer.h"
#include "ShaderManufacturer.h"
#include "WavefrontObjManufacturer.h"

std::optional<Products> AssetManufacturer::Manufacture( const std::filesystem::path& srcPath, const std::filesystem::path& destPath )
{
	IManufacturer* representative = nullptr;
	for ( const std::unique_ptr<IManufacturer>& manufacturer : m_manufacturers )
	{
		if ( manufacturer->IsSuitable( srcPath ) )
		{
			representative = manufacturer.get( );
			break;
		}
	}

	if ( representative == nullptr )
	{
		return {};
	}

	return representative->Manufacture( srcPath, destPath );
}

AssetManufacturer::AssetManufacturer( )
{
	m_manufacturers.emplace_back( std::make_unique<DDSManufacturer>( ) );
	m_manufacturers.emplace_back( std::make_unique<JsonManufacturer>( ) );
	m_manufacturers.emplace_back( std::make_unique<ShaderManufacturer>( ) );
	m_manufacturers.emplace_back( std::make_unique<WavefrontMtlManufacturer>( ) );
	m_manufacturers.emplace_back( std::make_unique<WavefrontObjManufacturer>( ) );
}
