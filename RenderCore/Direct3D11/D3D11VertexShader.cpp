#include "stdafx.h"
#include "D3D11VertexShader.h"

#include "../common.h"
#include "../CommonRenderer/IBuffer.h"
#include "../CommonRenderer/IRenderResource.h"
#include "../util_rendercore.h"

#include <D3D11.h>

void D3D11VertexShader::SetShader( )
{
	m_deviceContext.VSSetShader( m_pVertexShader.Get( ), nullptr, 0 );
	m_deviceContext.IASetInputLayout( m_pInputLayout.Get( ) );
}

void D3D11VertexShader::SetShaderResource( UINT slot, const IRenderResource* pResourceOrNull )
{
	ID3D11ShaderResourceView* srv = static_cast<ID3D11ShaderResourceView*>( pResourceOrNull ? pResourceOrNull->Get( ) : nullptr );
	m_deviceContext.VSSetShaderResources( slot, 1, &srv );
}

D3D11_INPUT_ELEMENT_DESC* D3D11VertexShader::CreateInputElementDesc ( const UINT num )
{
	m_numInputElement = num;

	delete[] m_pInputElementDesc;

	m_pInputElementDesc = new D3D11_INPUT_ELEMENT_DESC[m_numInputElement];

	return m_pInputElementDesc;
}

bool D3D11VertexShader::CreateShader( ID3D11Device& device, const void* byteCodePtr, const size_t byteCodeSize )
{
	if ( m_pInputElementDesc == nullptr )
	{
		return false;
	}

	bool result = SUCCEEDED( device.CreateInputLayout( m_pInputElementDesc,
		m_numInputElement,
		byteCodePtr,
		byteCodeSize,
		&m_pInputLayout ) );

	if ( result )
	{
		result = SUCCEEDED( device.CreateVertexShader(
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


D3D11VertexShader::D3D11VertexShader( ID3D11DeviceContext& deviceContext ) :
m_deviceContext( deviceContext )
{
}

D3D11VertexShader::~D3D11VertexShader ()
{
	delete[] m_pInputElementDesc;
}
