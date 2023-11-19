#include "TextureManufacturer.h"

#include "../DXGI/DxgiFlagConvertor.h"
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

		initializer.m_format = agl::ConvertDxgiFormatToFormat( meta.format );

		initializer.m_size = static_cast<uint32>( image.GetPixelsSize() );
		initializer.m_memory = image.GetPixels();

		for ( size_t mip = 0; mip < meta.mipLevels; ++mip )
		{
			for ( size_t item = 0; item < meta.arraySize; ++item )
			{
				size_t tIdx = meta.ComputeIndex( mip, item, 0 );
				auto subresources = image.GetImages();

				uint32 rowPitch = static_cast<uint32>( subresources[tIdx].rowPitch );
				uint32 slicePitch = static_cast<uint32>( subresources[tIdx].slicePitch );
				uint32 offset = static_cast<uint32>( subresources[tIdx].pixels - initializer.m_memory );

				initializer.m_sections.emplace_back( rowPitch, slicePitch, offset );
			}
		}

		return initializer;
	}
}

bool TextureManufacturer::IsSuitable( const std::filesystem::path& srcPath ) const
{
	fs::path extension = ToLower( srcPath.extension().generic_string() );
	return extension == fs::path( ".dds" )
		|| extension == fs::path( ".jpg" )
		|| extension == fs::path( ".tga" );
}

std::optional<Products> TextureManufacturer::Manufacture( [[maybe_unused]] const PathEnvironment& env, const std::filesystem::path& path ) const
{
	if ( fs::exists( path ) == false )
	{
		return {};
	}

	std::ifstream ddsFile;
	ddsFile.open( path, std::ios::binary | std::ios::ate );

	size_t fileSize = ddsFile.tellg();
	ddsFile.seekg( 0, std::ios::beg );

	if ( fileSize == 0 )
	{
		return {};
	}

	std::vector<char> buff( fileSize );
	ddsFile.read( buff.data(), fileSize );

	HRESULT hr;
	DirectX::TexMetadata meta;
	DirectX::ScratchImage image;

	fs::path extension = ToLower( path.extension().generic_string() );
	if ( extension == fs::path( ".dds" ) )
	{
		hr = DirectX::LoadFromDDSMemory( buff.data(), fileSize, DirectX::DDS_FLAGS_NONE, &meta, image );
	}
	else if ( extension == fs::path( ".tga" ) )
	{
		hr = DirectX::LoadFromTGAMemory( buff.data(), fileSize, &meta, image );
	}
	else
	{
		hr = DirectX::LoadFromWICMemory( buff.data(), fileSize, DirectX::WIC_FLAGS_NONE, &meta, image );
	}

	if ( FAILED( hr ) )
	{
		return {};
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

TextureManufacturer::TextureManufacturer()
{
	[[maybe_unused]] HRESULT hr = CoInitializeEx( nullptr, COINIT_MULTITHREADED );
}