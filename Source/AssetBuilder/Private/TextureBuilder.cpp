#include "TextureBuilder.h"

#include "DDSTexture.h"
#include "DirectXTexTool.h"

#include <DirectXTex.h>
#include <fstream>
#include <iostream>
#include <vector>

namespace fs = std::filesystem;

bool TextureBuilder::IsSuitable( const std::filesystem::path& srcPath ) const
{
	fs::path extension = ToLower( srcPath.extension().generic_string() );
	return extension == fs::path( ".dds" )
		|| extension == fs::path( ".jpg" )
		|| extension == fs::path( ".tga" );
}

std::optional<Products> TextureBuilder::Build( [[maybe_unused]] const PathEnvironment& env, const std::filesystem::path& path ) const
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

	std::vector<uint8> buff( fileSize );
	ddsFile.read( reinterpret_cast<char*>( buff.data() ), fileSize );

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

	if ( meta.mipLevels == 1 )
	{
		DirectX::ScratchImage mipChain;
		DirectX::TEX_FILTER_FLAGS filterFlag = DirectX::TEX_FILTER_CUBIC | DirectX::TEX_FILTER_FORCE_NON_WIC;

		if ( meta.IsCubemap() )
		{
			// Do Nothing
		}
		else if ( meta.IsVolumemap() )
		{
			hr = DirectX::GenerateMipMaps3D( image.GetImages(), image.GetImageCount(), image.GetMetadata(), filterFlag, 0, mipChain );
		}
		else
		{
			hr = DirectX::GenerateMipMaps( image.GetImages(), image.GetImageCount(), image.GetMetadata(), filterFlag, 0, mipChain );
		}

		if ( FAILED( hr ) )
		{
			std::cerr << std::format( "Failed to generate mip maps! - {}", path.generic_string() ) << std::endl;
			return {};
		}
		else if (mipChain.GetImageCount() != 0)
		{
			image = std::move( mipChain );
		}
	}

	rendercore::DDSTextureDesc desc = ConvertToBCTextureDesc( image );

	auto asset = std::make_unique<rendercore::DDSTexture>( desc );

	Products products;
	products.emplace_back( path.filename(), std::move( asset ) );
	return products;
}

TextureBuilder::TextureBuilder()
{
	[[maybe_unused]] HRESULT hr = CoInitializeEx( nullptr, COINIT_MULTITHREADED );
}
