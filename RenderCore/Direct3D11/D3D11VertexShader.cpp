#include "stdafx.h"
#include "D3D11VertexShader.h"

#include "../common.h"
#include "../IBuffer.h"
#include "../IRenderResource.h"
#include "../util_rendercore.h"

#include <D3D11.h>

void D3D11VertexShader::SetShader( )
{
	m_pDeviceContext->VSSetShader( m_pVertexShader.Get( ), nullptr, 0 );
	m_pDeviceContext->IASetInputLayout( m_pInputLayout.Get( ) );
}

void D3D11VertexShader::SetShaderResource( UINT slot, const IRenderResource* pResource )
{
	ID3D11ShaderResourceView* srv = static_cast<ID3D11ShaderResourceView*>( pResource ? pResource->Get( ) : nullptr );
	m_pDeviceContext->VSSetShaderResources( slot, 1, &srv );
}

void D3D11VertexShader::SetConstantBuffer( UINT slot, const IBuffer* pBuffer )
{
	if ( pBuffer )
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

D3D11VertexShader::D3D11VertexShader( ID3D11DeviceContext* pDeviceContext ) :
m_pDeviceContext( pDeviceContext )
{
	assert( m_pDeviceContext );
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
