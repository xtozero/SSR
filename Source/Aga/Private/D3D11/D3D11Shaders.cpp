#include "stdafx.h"
#include "D3D11Shaders.h"

#include "D3D11Api.h"

#include "ShaderPrameterMap.h"

#include <d3dcompiler.h>

SHADER_TYPE ConvertShaderVersionToType( UINT shaderVersion )
{
	D3D11_SHADER_VERSION_TYPE shaderType = static_cast<D3D11_SHADER_VERSION_TYPE>( D3D11_SHVER_GET_TYPE( shaderVersion ) );

	switch ( shaderType )
	{
	case D3D11_SHVER_PIXEL_SHADER:
		return SHADER_TYPE::PS;
		break;
	case D3D11_SHVER_VERTEX_SHADER:
		return SHADER_TYPE::VS;
		break;
	case D3D11_SHVER_GEOMETRY_SHADER:
		return SHADER_TYPE::GS;
		break;
	case D3D11_SHVER_HULL_SHADER:
		return SHADER_TYPE::HS;
		break;
	case D3D11_SHVER_DOMAIN_SHADER:
		return SHADER_TYPE::DS;
		break;
	case D3D11_SHVER_COMPUTE_SHADER:
		return SHADER_TYPE::CS;
		break;
	default:
		break;
	}

	return SHADER_TYPE::NONE;
}

void ExtractShaderParameters( const void* byteCode, std::size_t byteCodeSize, aga::ShaderParameterMap& parameterMap )
{
	ID3D11ShaderReflection* pReflector = nullptr;

	HRESULT hResult = D3DReflect( byteCode, byteCodeSize, IID_ID3D11ShaderReflection, reinterpret_cast<void**>( &pReflector ) );
	assert( SUCCEEDED( hResult ) );

	D3D11_SHADER_DESC shaderDesc = {};
	hResult = pReflector->GetDesc( &shaderDesc );
	assert( SUCCEEDED( hResult ) );

	SHADER_TYPE shaderType = ConvertShaderVersionToType( shaderDesc.Version );

	// Get the resources
	for ( UINT i = 0; i < shaderDesc.BoundResources; ++i )
	{
		D3D11_SHADER_INPUT_BIND_DESC bindDesc = {};
		hResult = pReflector->GetResourceBindingDesc( i, &bindDesc );
		assert( SUCCEEDED( hResult ) );

		aga::ShaderParameterType parameterType = aga::ShaderParameterType::ConstantBuffer;
		UINT parameterSize = 0;

		if ( bindDesc.Type == D3D_SIT_CBUFFER || bindDesc.Type == D3D_SIT_TBUFFER )
		{
			parameterType = aga::ShaderParameterType::ConstantBuffer;

			ID3D11ShaderReflectionConstantBuffer *constBufferReflection = pReflector->GetConstantBufferByName( bindDesc.Name );
			if ( constBufferReflection )
			{
				D3D11_SHADER_BUFFER_DESC shaderBuffDesc;
				constBufferReflection->GetDesc( &shaderBuffDesc );

				parameterSize = shaderBuffDesc.Size;

				for ( UINT j = 0; j < shaderBuffDesc.Variables; ++j )
				{
					ID3D11ShaderReflectionVariable* variableReflection = constBufferReflection->GetVariableByIndex( j );
					D3D11_SHADER_VARIABLE_DESC shaderVarDesc;
					variableReflection->GetDesc( &shaderVarDesc );

					parameterMap.AddParameter( shaderVarDesc.Name, shaderType, aga::ShaderParameterType::ConstantBufferValue, bindDesc.BindPoint, shaderVarDesc.StartOffset, shaderVarDesc.Size );
				}
			}
		}
		else if ( bindDesc.Type == D3D_SIT_TEXTURE )
		{
			parameterType = aga::ShaderParameterType::SRV;
		}
		else if ( bindDesc.Type == D3D_SIT_SAMPLER )
		{
			parameterType = aga::ShaderParameterType::Sampler;
		}
		else if ( bindDesc.Type == D3D_SIT_UAV_RWTYPED || bindDesc.Type == D3D_SIT_UAV_RWSTRUCTURED ||
			bindDesc.Type == D3D_SIT_UAV_RWBYTEADDRESS || bindDesc.Type == D3D_SIT_UAV_APPEND_STRUCTURED ||
			bindDesc.Type == D3D_SIT_UAV_CONSUME_STRUCTURED || bindDesc.Type == D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER )
		{
			parameterType = aga::ShaderParameterType::UAV;
		}
		else if ( bindDesc.Type == D3D_SIT_STRUCTURED || bindDesc.Type == D3D_SIT_BYTEADDRESS )
		{
			parameterType = aga::ShaderParameterType::SRV;
		}
		else
		{
			assert( false && "Unexpected case" );
		}

		parameterMap.AddParameter( bindDesc.Name, shaderType, parameterType, bindDesc.BindPoint, 0, parameterSize );
	}

	pReflector->Release( );
}

void BuildShaderParameterInfo( const std::map<std::string, aga::ShaderParameter>& parameterMap, aga::ShaderParameterInfo& parameterInfo )
{
	for ( int parameterType = static_cast<int>( aga::ShaderParameterType::ConstantBuffer ); parameterType < static_cast<int>( aga::ShaderParameterType::Count ); ++parameterType )
	{
		auto curParameterType = static_cast<aga::ShaderParameterType>( parameterType );
		std::size_t count = 0;

		for ( auto iter = parameterMap.begin( ); iter != parameterMap.end( ); ++iter )
		{
			if ( iter->second.m_type == curParameterType )
			{
				++count;
			}
		}

		std::vector<aga::ShaderParameter>* shaderParameters = nullptr;

		if ( curParameterType == aga::ShaderParameterType::ConstantBuffer )
		{
			shaderParameters = &parameterInfo.m_constantBuffers;
		}
		else if ( curParameterType == aga::ShaderParameterType::SRV )
		{
			shaderParameters = &parameterInfo.m_srvs;
		}
		else if ( curParameterType == aga::ShaderParameterType::UAV )
		{
			shaderParameters = &parameterInfo.m_uavs;
		}
		else if ( curParameterType == aga::ShaderParameterType::Sampler )
		{
			shaderParameters = &parameterInfo.m_samplers;
		}
		else
		{
			continue;
		}

		shaderParameters->reserve( count );

		for ( auto iter = parameterMap.begin( ); iter != parameterMap.end( ); ++iter )
		{
			if ( iter->second.m_type == curParameterType )
			{
				shaderParameters->push_back( iter->second );
			}
		}
	}
}

namespace aga
{
	void D3D11VertexShader::InitResource( )
	{
		bool result = SUCCEEDED( D3D11Device( ).CreateVertexShader( m_byteCode, m_byteCodeSize, nullptr, &m_pResource ) );
		assert( result );
	}

	void D3D11VertexShader::FreeResource( )
	{
		if ( m_pResource )
		{
			m_pResource->Release( );
			m_pResource = nullptr;
		}
	}

	void D3D11PixelShader::InitResource( )
	{
		bool result = SUCCEEDED( D3D11Device( ).CreatePixelShader( m_byteCode, m_byteCodeSize, nullptr, &m_pResource ) );
		assert( result );
	}

	void D3D11PixelShader::FreeResource( )
	{
		if ( m_pResource )
		{
			m_pResource->Release( );
			m_pResource = nullptr;
		}
	}

	void D3D11ComputeShader::InitResource( )
	{
		bool result = SUCCEEDED( D3D11Device( ).CreateComputeShader( m_byteCode, m_byteCodeSize, nullptr, &m_pResource ) );
		assert( result );
	}

	void D3D11ComputeShader::FreeResource( )
	{
		if ( m_pResource )
		{
			m_pResource->Release( );
			m_pResource = nullptr;
		}
	}

}
