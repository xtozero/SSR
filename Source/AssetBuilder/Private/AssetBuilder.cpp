#include "AssetBuilder.h"

#include "IAsyncLoadableAsset.h"
#include "JsonBuilder.h"
#include "ShaderBuilder.h"
#include "TextureBuilder.h"
#include "WavefrontObjBuilder.h"

namespace fs = std::filesystem;

std::optional<Products> AssetBuilder::Build( const PathEnvironment& env, const std::filesystem::path& path, uint64 fileHash ) const
{
	IAssetBuilder* representative = nullptr;
	for ( const std::unique_ptr<IAssetBuilder>& assetBuilder : m_assetBuilders )
	{
		if ( assetBuilder->IsSuitable( path ) )
		{
			representative = assetBuilder.get();
			break;
		}
	}

	if ( representative == nullptr )
	{
		return {};
	}

	auto products = representative->Build( env, path );
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

void AssetBuilder::Initialize()
{
	m_assetBuilders.emplace_back( std::make_unique<TextureBuilder>() );
	m_assetBuilders.emplace_back( std::make_unique<JsonBuilder>() );
	m_assetBuilders.emplace_back( std::make_unique<ShaderBuilder>() );
	m_assetBuilders.emplace_back( std::make_unique<WavefrontMtlBuilder>() );
	m_assetBuilders.emplace_back( std::make_unique<WavefrontObjBuilder>() );

	for ( std::unique_ptr<IAssetBuilder>& assetBuilder : m_assetBuilders )
	{
		bool succeeded = assetBuilder->Initialize();
		assert( succeeded );
	}
}
