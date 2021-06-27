#include "DDSManufacturer.h"

#include "D3D11FlagConvertor.h"
#include "DDSTexture.h"
#include "DirectXTex.h"

#include <fstream>
#include <vector>

namespace fs = std::filesystem;

namespace
{
	std::size_t ConvertToBCTextureDemension( DirectX::TEX_DIMENSION dimension )
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

	DDSTextureInitializer ConvertToBCTextureInitializer( const DirectX::ScratchImage& image )
	{
		DDSTextureInitializer initializer;

		const DirectX::TexMetadata& meta = image.GetMetadata( );
		initializer.m_width = meta.width;
		initializer.m_height = meta.height;
		initializer.m_depth = meta.depth;
		initializer.m_arraySize = meta.arraySize;
		initializer.m_mipLevels = meta.mipLevels;

		initializer.m_isCubeMap = meta.IsCubemap( );
		initializer.m_demension = ConvertToBCTextureDemension( meta.dimension );

		initializer.m_format = ConvertDxgiFormatToFormat( meta.format );

		initializer.m_size = image.GetPixelsSize( );
		initializer.m_memory = image.GetPixels( );

		for ( std::size_t i = 0; i < image.GetImageCount( ); ++i )
		{
			auto subresources = image.GetImages( );
			initializer.m_sections.emplace_back( subresources[i].rowPitch, subresources[i].slicePitch, subresources[i].pixels - initializer.m_memory );
		}

		return initializer;
	}
}

bool DDSManufacturer::IsSuitable( const std::filesystem::path& srcPath ) const
{
	return srcPath.extension() == fs::path( ".dds" );
}

std::optional<Products> DDSManufacturer::Manufacture( const std::filesystem::path& srcPath, [[maybe_unused]] const std::filesystem::path& destPath ) const
{
	if ( fs::exists( srcPath ) == false )
	{
		return { };
	}

	std::ifstream ddsFile;
	ddsFile.open( srcPath, std::ios::binary | std::ios::ate );

	std::size_t fileSize = ddsFile.tellg( );
	ddsFile.seekg( 0, std::ios::beg );

	if ( fileSize == 0 )
	{
		return { };
	}

	std::vector<char> buff( fileSize );
	ddsFile.read( buff.data( ), fileSize );

	DirectX::TexMetadata meta;
	DirectX::ScratchImage image;
	HRESULT hr = DirectX::LoadFromDDSMemory( buff.data( ), fileSize, DirectX::DDS_FLAGS_NONE, &meta, image );

	if ( FAILED( hr ) )
	{
		return { };
	}

	DDSTextureInitializer initializer = ConvertToBCTextureInitializer( image );

	DDSTexture asset( initializer );

	Archive ar;
	asset.Serialize( ar );

	Products products;
	products.emplace_back( srcPath.filename( ), std::move( ar ) );
	return products;
}
