#include "stdafx.h"
#include "D3D11Shaders.h"

#include "D3D11Api.h"

#include "ShaderPrameterMap.h"

#include <d3dcompiler.h>

void ExtractShaderParameters( const void* byteCode, std::size_t byteCodeSize, ShaderParameterMap& parameterMap )
{
	ID3D11ShaderReflection* pReflector = nullptr;

	HRESULT hResult = D3DReflect( byteCode, byteCodeSize, IID_ID3D11ShaderReflection, reinterpret_cast<void**>( &pReflector ) );
	assert( SUCCEEDED( hResult ) );

	D3D11_SHADER_DESC shaderDesc = {};
	hResult = pReflector->GetDesc( &shaderDesc );
	assert( SUCCEEDED( hResult ) );

	// Get the resources
	for ( UINT i = 0; i < shaderDesc.BoundResources; ++i )
	{
		D3D11_SHADER_INPUT_BIND_DESC bindDesc = {};
		hResult = pReflector->GetResourceBindingDesc( i, &bindDesc );
		assert( SUCCEEDED( hResult ) );

		ShaderParameterType parameterType = ShaderParameterType::ConstantBuffer;

		if ( bindDesc.Type == D3D_SIT_CBUFFER || bindDesc.Type == D3D_SIT_TBUFFER )
		{
			parameterType = ShaderParameterType::ConstantBuffer;
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

		parameterMap.AddParameter( bindDesc.Name, parameterType, bindDesc.BindPoint );
	}

	pReflector->Release( );
}

void BuildShaderParameterInfo( const std::map<std::string, ShaderParameter>& parameterMap, ShaderParameterInfo& parameterInfo )
{
	for ( int parameterType = static_cast<int>( ShaderParameterType::ConstantBuffer ); parameterType < static_cast<int>( ShaderParameterType::Count ); ++parameterType )
	{
		ShaderParameterType curParameterType = static_cast<ShaderParameterType>( parameterType );
		std::size_t count = 0;

		for ( auto iter = parameterMap.begin( ); iter != parameterMap.end( ); ++iter )
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
		else
		{
			shaderParameters = &parameterInfo.m_samplers;
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

//void CD3D11VertexShader::InitResource( )
//{
//	bool result = SUCCEEDED( D3D11Device( ).CreateVertexShader( m_byteCode, m_byteCodeSize, nullptr, m_pResource.GetAddressOf( ) ) );
//	assert( result );
//}
//
//void CD3D11GeometryShader::InitResource( )
//{
//	bool result = SUCCEEDED( D3D11Device( ).CreateGeometryShader( m_byteCode, m_byteCodeSize, nullptr, m_pResource.GetAddressOf( ) ) );
//	assert( result );
//}
//
//void CD3D11PixelShader::InitResource( )
//{
//	bool result = SUCCEEDED( D3D11Device( ).CreatePixelShader( m_byteCode, m_byteCodeSize, nullptr, m_pResource.GetAddressOf( ) ) );
//
//	assert( result );
//}
//
//void CD3D11ComputeShader::InitResource( )
//{
//	bool result = SUCCEEDED( D3D11Device( ).CreateComputeShader( m_byteCode, m_byteCodeSize, nullptr, m_pResource.GetAddressOf( ) ) );
//	assert( result );
//}

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
