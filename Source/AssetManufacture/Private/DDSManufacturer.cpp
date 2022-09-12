#include "DDSManufacturer.h"

#include "D3D11FlagConvertor.h"
#include "DDSTexture.h"
#include "DirectXTex.h"

#include <fstream>
#include <vector>

namespace fs = std::filesystem;

namespace
{
	uint32 ConvertToBCTextureDemension( DirectX::TEX_DIMENSION dimension )
	{
		switch ( dimension )
		{
		case DirectX::TEX_DIMENSION_TEXTURE1D:
			return 1;
			break;
		case DirectX::TEX_DIMENSION_TEXTURE2D:
			return 2;
			break;
		case DirectX::TEX_DIMENSION_TEXTURE3D:
			return 3;
			break;
		default:
			break;
		}

		return 0;
	}

	rendercore::DDSTextureInitializer ConvertToBCTextureInitializer( const DirectX::ScratchImage& image )
	{
		rendercore::DDSTextureInitializer initializer;

		const DirectX::TexMetadata& meta = image.GetMetadata();
		initializer.m_width = static_cast<uint32>( meta.width );
		initializer.m_height = static_cast<uint32>( meta.height );
		initializer.m_depth = static_cast<uint32>( meta.depth );
		initializer.m_arraySize = static_cast<uint32>( meta.arraySize );
		initializer.m_mipLevels = static_cast<uint32>( meta.mipLevels );

		initializer.m_isCubeMap = meta.IsCubemap();
		initializer.m_demension = ConvertToBCTextureDemension( meta.dimension );

		initializer.m_format = ConvertDxgiFormatToFormat( meta.format );

		initializer.m_size = static_cast<uint32>( image.GetPixelsSize() );
		initializer.m_memory = image.GetPixels();

		for ( size_t i = 0; i < image.GetImageCount(); ++i )
		{
			auto subresources = image.GetImages();

			uint32 rowPitch = static_cast<uint32>( subresources[i].rowPitch );
			uint32 slicePitch = static_cast<uint32>( subresources[i].slicePitch );
			uint32 offset = static_cast<uint32>( subresources[i].pixels - initializer.m_memory );

			initializer.m_sections.emplace_back( rowPitch, slicePitch, offset );
		}

		return initializer;
	}
}

bool DDSManufacturer::IsSuitable( const std::filesystem::path& srcPath ) const
{
	return srcPath.extension() == fs::path( ".dds" );
}

std::optional<Products> DDSManufacturer::Manufacture( [[maybe_unused]] const PathEnvironment& env, const std::filesystem::path& path ) const
{
	if ( fs::exists( path ) == false )
	{
		return { };
	}

	std::ifstream ddsFile;
	ddsFile.open( path, std::ios::binary | std::ios::ate );

	size_t fileSize = ddsFile.tellg();
	ddsFile.seekg( 0, std::ios::beg );

	if ( fileSize == 0 )
	{
		return { };
	}

	std::vector<char> buff( fileSize );
	ddsFile.read( buff.data(), fileSize );

	DirectX::TexMetadata meta;
	DirectX::ScratchImage image;
	HRESULT hr = DirectX::LoadFromDDSMemory( buff.data(), fileSize, DirectX::DDS_FLAGS_NONE, &meta, image );

	if ( FAILED( hr ) )
	{
		return { };
	}

	rendercore::DDSTextureInitializer initializer = ConvertToBCTextureInitializer( image );

	rendercore::DDSTexture asset( initializer );
	asset.SetLastWriteTime( fs::last_write_time( path ) );

	Archive ar;
	asset.Serialize( ar );

	Products products;
	products.emplace_back( path.filename(), std::move( ar ) );
	return products;
}
