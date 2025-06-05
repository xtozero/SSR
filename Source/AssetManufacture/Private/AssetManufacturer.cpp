#include "AssetManufacturer.h"

#include "IAsyncLoadableAsset.h"
#include "JsonManufacturer.h"
#include "ShaderManufacturer.h"
#include "TextureManufacturer.h"
#include "WavefrontObjManufacturer.h"

namespace fs = std::filesystem;

std::optional<Products> AssetManufacturer::Manufacture( const PathEnvironment& env, const std::filesystem::path& path, uint64 fileHash ) const
{
	IManufacturer* representative = nullptr;
	for ( const std::unique_ptr<IManufacturer>& manufacturer : m_manufacturers )
	{
		if ( manufacturer->IsSuitable( path ) )
		{
			representative = manufacturer.get();
			break;
		}
	}

	if ( representative == nullptr )
	{
		return {};
	}

	auto products = representative->Manufacture( env, path );
	if ( products.has_value() )
	{
		for ( const auto& product : *products )
		{
			if ( auto asyncLoadableAsset = Cast<AsyncLoadableAsset>( product.second.get() ) )
			{
				asyncLoadableAsset->SetLastWriteTime( fs::last_write_time( path ) );
				asyncLoadableAsset->SetFileHash( fileHash );
			}
		}
	}

	return products;
}

void AssetManufacturer::Initialize()
{
	m_manufacturers.emplace_back( std::make_unique<TextureManufacturer>() );
	m_manufacturers.emplace_back( std::make_unique<JsonManufacturer>() );
	m_manufacturers.emplace_back( std::make_unique<ShaderManufacturer>() );
	m_manufacturers.emplace_back( std::make_unique<WavefrontMtlManufacturer>() );
	m_manufacturers.emplace_back( std::make_unique<WavefrontObjManufacturer>() );

	for ( std::unique_ptr<IManufacturer>& manufacturer : m_manufacturers )
	{
		bool succeeded = manufacturer->Initialize();
		assert( succeeded );
	}
}
