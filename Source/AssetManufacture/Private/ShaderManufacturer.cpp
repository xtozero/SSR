#include "ShaderManufacturer.h"

#include "Shader.h"
#include "../D3D11/D3D11Shaders.h"

#include <fstream>
#include <d3dcompiler.h>
#include <wrl/client.h>

namespace fs = std::filesystem;

namespace
{
	bool ValidateShaderAsset( const AsyncLoadableAsset* asset, const Archive& ar )
	{
		Archive rAr( ar.Data( ), ar.Size( ) );
		std::size_t assetID = 0;
		rAr << assetID;

		if ( assetID == PixelShader::ID )
		{
			PixelShader ps;
			ps.Serialize( rAr );

			return ( ps == *reinterpret_cast<const PixelShader*>( asset ) );
		}
		else if ( assetID == VertexShader::ID )
		{
			VertexShader vs;
			vs.Serialize( rAr );

			return ( vs == *reinterpret_cast<const VertexShader*>( asset ) );
		}

		return false;
	}
}

bool ShaderManufacturer::IsSuitable( const std::filesystem::path& srcPath ) const
{
	return srcPath.extension( ) == fs::path( ".cso" );
}

std::optional<Products> ShaderManufacturer::Manufacture( const std::filesystem::path& srcPath, [[maybe_unused]] const std::filesystem::path& destPath ) const
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

	aga::ShaderParameterMap parameterMap;
	ExtractShaderParameters( byteCode.Data( ), size, parameterMap );

	ShaderBase* shader = nullptr;
	switch ( shaderType )
	{
	case D3D11_SHVER_PIXEL_SHADER:
	{
		shader = new PixelShader( std::move( byteCode ) );
		break;
	}
	case D3D11_SHVER_VERTEX_SHADER:
	{
		shader = new VertexShader( std::move( byteCode ) );
		break;
	}
	// Reservation
	//case D3D11_SHVER_GEOMETRY_SHADER:
	//	break;
	//case D3D11_SHVER_HULL_SHADER:
	//	break;
	//case D3D11_SHVER_DOMAIN_SHADER:
	//	break;
	case D3D11_SHVER_COMPUTE_SHADER:
	{
		shader = new ComputeShader( std::move( byteCode ) );
		break;
	}
	//case D3D11_SHVER_RESERVED0:
	//	break;
	default:
		break;
	}

	if ( shader == nullptr )
	{
		return {};
	}

	BuildShaderParameterInfo( parameterMap.GetParameterMap( ), shader->ParameterInfo( ) );
	shader->ParameterMap( ) = parameterMap;

	Archive ar;
	shader->Serialize( ar );

#ifdef ASSET_VALIDATE
	if ( ValidateShaderAsset( shader, ar ) == false )
	{
		DebugBreak( );
	}
#endif

	Products products;
	products.emplace_back( srcPath.filename( ), std::move( ar ) );
	return products;
}
