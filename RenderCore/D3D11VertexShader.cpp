#include "stdafx.h"

#include "common.h"
#include "D3D11VertexShader.h"

bool D3D11VertexShader::CreateShader ( const D3D11_CREATE_VS_TRAIT& trait )
{
	HRESULT hr;

	D3DX11CompileFromFile ( trait.m_pFilePath,
		NULL,
		NULL,
		"main",
		trait.m_pProfile,
		0,
		0,
		NULL,
		&m_shaderBlob,
		NULL,
		&hr );

	if ( SUCCEEDED ( hr ) )
	{
		if ( SUCCEEDED ( trait.m_pDevice->CreateVertexShader (
			m_shaderBlob->GetBufferPointer ( ),
			m_shaderBlob->GetBufferSize ( ),
			NULL,
			&m_pVertexShader ) ) )
		{
			return true;
		}
	}

	return false;
}

void D3D11VertexShader::SetShader ( const D3D11_SET_VS_TRAIT& trait )
{
	trait.m_pDeviceContext->VSSetShader ( m_pVertexShader, NULL, 0 );
}

D3D11VertexShader::D3D11VertexShader ( ) : m_pVertexShader ( NULL )
{
}

D3D11VertexShader::~D3D11VertexShader ()
{
	SAFE_RELEASE ( m_pVertexShader );
}