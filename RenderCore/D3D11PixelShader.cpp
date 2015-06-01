#include "stdafx.h"

#include "common.h"
#include "D3D11PixelShader.h"

bool D3D11PixelShader::CreateShader ( const D3D11_CREATE_PS_TRAIT& trait )
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
		if ( SUCCEEDED ( trait.m_pDevice->CreatePixelShader (
			m_shaderBlob->GetBufferPointer ( ),
			m_shaderBlob->GetBufferSize ( ),
			NULL,
			&m_pPixelShader ) ) )
		{
			return true;
		}
	}

	return false;
}

void D3D11PixelShader::SetShader ( const D3D11_SET_PS_TRAIT& trait )
{
	trait.m_pDeviceContext->PSSetShader ( m_pPixelShader, NULL, 0 );
}

D3D11PixelShader::D3D11PixelShader ()
{
}

D3D11PixelShader::~D3D11PixelShader ()
{
	SAFE_RELEASE ( m_pPixelShader );
}
