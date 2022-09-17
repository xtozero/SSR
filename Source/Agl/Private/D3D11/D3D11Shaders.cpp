#include "stdafx.h"
#include "D3D11Shaders.h"

#include "D3D11Api.h"

#include "ShaderParameterMap.h"

using ::agl::ShaderType;

namespace
{
	ShaderType ConvertShaderVersionToType( uint32 shaderVersion )
	{
		D3D11_SHADER_VERSION_TYPE shaderType = static_cast<D3D11_SHADER_VERSION_TYPE>( D3D11_SHVER_GET_TYPE( shaderVersion ) );

		switch ( shaderType )
		{
		case D3D11_SHVER_PIXEL_SHADER:
			return ShaderType::PS;
			break;
		case D3D11_SHVER_VERTEX_SHADER:
			return ShaderType::VS;
			break;
		case D3D11_SHVER_GEOMETRY_SHADER:
			return ShaderType::GS;
			break;
		case D3D11_SHVER_HULL_SHADER:
			return ShaderType::HS;
			break;
		case D3D11_SHVER_DOMAIN_SHADER:
			return ShaderType::DS;
			break;
		case D3D11_SHVER_COMPUTE_SHADER:
			return ShaderType::CS;
			break;
		default:
			break;
		}

		return ShaderType::None;
	}
}

namespace agl
{
	void ExtractShaderParameters( ID3D11ShaderReflection* pReflector, ShaderParameterMap& parameterMap )
	{
		D3D11_SHADER_DESC shaderDesc = {};
		HRESULT hResult = pReflector->GetDesc( &shaderDesc );
		assert( SUCCEEDED( hResult ) );

		ShaderType shaderType = ConvertShaderVersionToType( shaderDesc.Version );

		// Get the resources
		for ( uint32 i = 0; i < shaderDesc.BoundResources; ++i )
		{
			D3D11_SHADER_INPUT_BIND_DESC bindDesc = {};
			hResult = pReflector->GetResourceBindingDesc( i, &bindDesc );
			assert( SUCCEEDED( hResult ) );

			ShaderParameterType parameterType = ShaderParameterType::ConstantBuffer;
			uint32 parameterSize = 0;

			if ( bindDesc.Type == D3D_SIT_CBUFFER || bindDesc.Type == D3D_SIT_TBUFFER )
			{
				parameterType = ShaderParameterType::ConstantBuffer;

				ID3D11ShaderReflectionConstantBuffer* constBufferReflection = pReflector->GetConstantBufferByName( bindDesc.Name );
				if ( constBufferReflection )
				{
					D3D11_SHADER_BUFFER_DESC shaderBuffDesc;
					constBufferReflection->GetDesc( &shaderBuffDesc );

					parameterSize = shaderBuffDesc.Size;

					for ( uint32 j = 0; j < shaderBuffDesc.Variables; ++j )
					{
						ID3D11ShaderReflectionVariable* variableReflection = constBufferReflection->GetVariableByIndex( j );
						D3D11_SHADER_VARIABLE_DESC shaderVarDesc;
						variableReflection->GetDesc( &shaderVarDesc );

						parameterMap.AddParameter( shaderVarDesc.Name, shaderType, ShaderParameterType::ConstantBufferValue, bindDesc.BindPoint, shaderVarDesc.StartOffset, shaderVarDesc.Size );
					}
				}
			}
			else if ( bindDesc.Type == D3D_SIT_TEXTURE )
			{
				parameterType = ShaderParameterType::SRV;
			}
			else if ( bindDesc.Type == D3D_SIT_SAMPLER )
			{
				parameterType = ShaderParameterType::Sampler;
			}
			else if ( bindDesc.Type == D3D_SIT_UAV_RWTYPED || bindDesc.Type == D3D_SIT_UAV_RWSTRUCTURED ||
				bindDesc.Type == D3D_SIT_UAV_RWBYTEADDRESS || bindDesc.Type == D3D_SIT_UAV_APPEND_STRUCTURED ||
				bindDesc.Type == D3D_SIT_UAV_CONSUME_STRUCTURED || bindDesc.Type == D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER )
			{
				parameterType = ShaderParameterType::UAV;
			}
			else if ( bindDesc.Type == D3D_SIT_STRUCTURED || bindDesc.Type == D3D_SIT_BYTEADDRESS )
			{
				parameterType = ShaderParameterType::SRV;
			}
			else
			{
				assert( false && "Unexpected case" );
			}

			parameterMap.AddParameter( bindDesc.Name, shaderType, parameterType, bindDesc.BindPoint, 0, parameterSize );
		}
	}

	void BuildShaderParameterInfo( const std::map<Name, ShaderParameter>& parameterMap, ShaderParameterInfo& parameterInfo )
	{
		for ( uint32 i = 0; i < static_cast<uint32>( ShaderParameterType::Count ); ++i )
		{
			auto curParameterType = static_cast<ShaderParameterType>( i );
			size_t count = 0;

			for ( auto iter = parameterMap.begin(); iter != parameterMap.end(); ++iter )
			{
				if ( iter->second.m_type == curParameterType )
				{
					++count;
				}
			}

			std::vector<ShaderParameter>* shaderParameters = nullptr;

			if ( curParameterType == ShaderParameterType::ConstantBuffer )
			{
				shaderParameters = &parameterInfo.m_constantBuffers;
			}
			else if ( curParameterType == ShaderParameterType::SRV )
			{
				shaderParameters = &parameterInfo.m_srvs;
			}
			else if ( curParameterType == ShaderParameterType::UAV )
			{
				shaderParameters = &parameterInfo.m_uavs;
			}
			else if ( curParameterType == ShaderParameterType::Sampler )
			{
				shaderParameters = &parameterInfo.m_samplers;
			}
			else
			{
				continue;
			}

			shaderParameters->reserve( count );

			for ( auto iter = parameterMap.begin(); iter != parameterMap.end(); ++iter )
			{
				if ( iter->second.m_type == curParameterType )
				{
					shaderParameters->push_back( iter->second );
				}
			}
		}
	}

	void D3D11VertexShader::InitResource()
	{
		[[maybe_unused]] bool result = SUCCEEDED( D3D11Device().CreateVertexShader( m_byteCode, m_byteCodeSize, nullptr, &m_pResource ) );
		assert( result );
	}

	void D3D11VertexShader::FreeResource()
	{
		if ( m_pResource )
		{
			m_pResource->Release();
			m_pResource = nullptr;
		}
	}

	void D3D11GeometryShader::InitResource()
	{
		[[maybe_unused]] bool result = SUCCEEDED( D3D11Device().CreateGeometryShader( m_byteCode, m_byteCodeSize, nullptr, &m_pResource ) );
		assert( result );
	}

	void D3D11GeometryShader::FreeResource()
	{
		if ( m_pResource )
		{
			m_pResource->Release();
			m_pResource = nullptr;
		}
	}

	void D3D11PixelShader::InitResource()
	{
		[[maybe_unused]] bool result = SUCCEEDED( D3D11Device().CreatePixelShader( m_byteCode, m_byteCodeSize, nullptr, &m_pResource ) );
		assert( result );
	}

	void D3D11PixelShader::FreeResource()
	{
		if ( m_pResource )
		{
			m_pResource->Release();
			m_pResource = nullptr;
		}
	}

	void D3D11ComputeShader::InitResource()
	{
		[[maybe_unused]] bool result = SUCCEEDED( D3D11Device().CreateComputeShader( m_byteCode, m_byteCodeSize, nullptr, &m_pResource ) );
		assert( result );
	}

	void D3D11ComputeShader::FreeResource()
	{
		if ( m_pResource )
		{
			m_pResource->Release();
			m_pResource = nullptr;
		}
	}
}
