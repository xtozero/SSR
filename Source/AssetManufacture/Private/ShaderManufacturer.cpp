#include "ShaderManufacturer.h"

#include "Rendering/ShaderAsset.h"

#include <fstream>
#include <d3dcompiler.h>
#include <wrl/client.h>

namespace fs = std::filesystem;

bool ShaderManufacturer::IsSuitable( const std::filesystem::path& srcPath ) const
{
	return srcPath.extension( ) == fs::path( ".cso" );
}

std::optional<Products> ShaderManufacturer::Manufacture( const std::filesystem::path& srcPath, [[maybe_unused]] const std::filesystem::path* destRootHint ) const
{
	if ( fs::exists( srcPath ) == false )
	{
		return {};
	}

	std::ifstream compiledShader( srcPath, std::ios::ate | std::ios::binary );
	std::size_t size = compiledShader.tellg( );
	compiledShader.seekg( 0 );

	BinaryChunk byteCode( size );
	compiledShader.read( byteCode.Data( ), size );

	Microsoft::WRL::ComPtr<ID3D11ShaderReflection> pShaderReflection = nullptr;
	HRESULT hr = D3DReflect( byteCode.Data( ), size, IID_PPV_ARGS( &pShaderReflection ) );
	if ( FAILED( hr ) )
	{
		return {};
	}

	D3D11_SHADER_DESC desc;
	hr = pShaderReflection->GetDesc( &desc );
	if ( FAILED( hr ) )
	{
		return {};
	}

	D3D11_SHADER_VERSION_TYPE shaderType = static_cast<D3D11_SHADER_VERSION_TYPE>( D3D11_SHVER_GET_TYPE( desc.Version ) );

	int assetID = -1;
	switch ( shaderType )
	{
	case D3D11_SHVER_PIXEL_SHADER:
		assetID = PixelShaderAsset::ID;
		break;
	case D3D11_SHVER_VERTEX_SHADER:
		assetID = VertexShaderAsset::ID;
		break;
	// Reservation
	//case D3D11_SHVER_GEOMETRY_SHADER:
	//	break;
	//case D3D11_SHVER_HULL_SHADER:
	//	break;
	//case D3D11_SHVER_DOMAIN_SHADER:
	//	break;
	//case D3D11_SHVER_COMPUTE_SHADER:
	//	break;
	//case D3D11_SHVER_RESERVED0:
	//	break;
	default:
		return {};
		break;
	}

	Archive ar;
	ar << assetID;
	ar << byteCode;

#ifdef ASSET_VALIDATE
	Archive rAr( ar.Data( ), ar.Size( ) );
	int rAssetID;
	BinaryChunk rByteCode( 0 );

	rAr << rAssetID;
	rAr << rByteCode;

	if ( std::memcmp( byteCode.Data( ), rByteCode.Data( ), rByteCode.Size( ) ) == 0 )
	{
		DebugBreak( );
	}
#endif

	Products products;
	products.emplace_back( srcPath.filename( ), std::move( ar ) );
	return products;
}
