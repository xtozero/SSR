#include "TextureManufacturer.h"

#include "DDSTexture.h"
#include "DirectXTex.h"
#include "DirectXTexTool.h"

#include <fstream>
#include <vector>

namespace fs = std::filesystem;

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
