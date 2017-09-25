#include "stdafx.h"

#include "common.h"
#include "D3D11VertexShader.h"
#include "IBuffer.h"
#include "IShaderResource.h"
#include "util_rendercore.h"

#include <D3D11.h>

void D3D11VertexShader::SetShader ( ID3D11DeviceContext* pDeviceContext )
{
	if ( pDeviceContext )
	{
		pDeviceContext->VSSetShader( m_pVertexShader.Get( ), nullptr, 0 );
		pDeviceContext->IASetInputLayout( m_pInputLayout.Get( ) );
	}
}

void D3D11VertexShader::SetShaderResource( ID3D11DeviceContext* pDeviceContext, UINT slot, const IShaderResource* pResource )
{
	if ( pDeviceContext )
	{
		ID3D11ShaderResourceView* srv = nullptr;

		if ( pResource && pResource->Get( ) )
		{
			srv = pResource->Get( );
		}

		pDeviceContext->VSSetShaderResources( slot, 1, &srv );
	}
}

void D3D11VertexShader::SetConstantBuffer( ID3D11DeviceContext * pDeviceContext, UINT slot, const IBuffer * pBuffer )
{
	if ( pDeviceContext && pBuffer )
	{
		pBuffer->SetVSBuffer( slot );
	}
}

D3D11_INPUT_ELEMENT_DESC* D3D11VertexShader::CreateInputElementDesc ( const UINT num )
{
	m_numInputElement = num;

	SAFE_ARRAY_DELETE ( m_pInputElementDesc );

	m_pInputElementDesc = new D3D11_INPUT_ELEMENT_DESC[m_numInputElement];

	return m_pInputElementDesc;
}

D3D11VertexShader::D3D11VertexShader( ) : m_pVertexShader( nullptr ),
m_pInputElementDesc( nullptr ),
m_numInputElement ( 0 ),
m_pInputLayout( nullptr )
{
}

D3D11VertexShader::~D3D11VertexShader ()
{
	SAFE_ARRAY_DELETE ( m_pInputElementDesc );
}

bool D3D11VertexShader::CreateShaderInternal( ID3D11Device* pDevice, const void * byteCodePtr, const size_t byteCodeSize )
{
	if ( m_pInputElementDesc == nullptr )
	{
		return false;
	}

	bool result = SUCCEEDED( pDevice->CreateInputLayout( m_pInputElementDesc,
		m_numInputElement,
		byteCodePtr,
		byteCodeSize,
		&m_pInputLayout ) );

	if ( result )
	{
		result = SUCCEEDED( pDevice->CreateVertexShader(
			byteCodePtr,
			byteCodeSize,
			nullptr,
			&m_pVertexShader ) );
	}

#ifdef _DEBUG
	SetDebugName( m_pVertexShader.Get( ), "Vertex Shader" );
#endif

	return result;
}
