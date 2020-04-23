#include "stdafx.h"
#include "D3D11Shaders.h"

#include "D3D11Api.h"

void CD3D11VertexShader::InitResource( )
{
	bool result = SUCCEEDED( D3D11Device( ).CreateVertexShader( m_byteCodePtr, m_byteCodeSize, nullptr, m_pResource.GetAddressOf( ) ) );
	assert( result );
}

void CD3D11GeometryShader::InitResource( )
{
	bool result = SUCCEEDED( D3D11Device( ).CreateGeometryShader( m_byteCodePtr, m_byteCodeSize, nullptr, m_pResource.GetAddressOf( ) ) );
	assert( result );
}

void CD3D11PixelShader::InitResource( )
{
	bool result = SUCCEEDED( D3D11Device( ).CreatePixelShader( m_byteCodePtr, m_byteCodeSize, nullptr, m_pResource.GetAddressOf( ) ) );

	assert( result );
}

void CD3D11ComputeShader::InitResource( )
{
	bool result = SUCCEEDED( D3D11Device( ).CreateComputeShader( m_byteCodePtr, m_byteCodeSize, nullptr, m_pResource.GetAddressOf( ) ) );
	assert( result );
}