#include "stdafx.h"

#include "common.h"
#include "D3D11PixelShader.h"

#include <D3D11.h>

bool D3D11PixelShader::CreateShader ( ID3D11Device* pDevice, const TCHAR* pFilePath, const char* pProfile )
{
	if ( pDevice )
	{
		Microsoft::WRL::ComPtr<ID3D10Blob> shaderBlob = GetShaderBlob( pFilePath, pProfile );

		if ( shaderBlob )
		{
			bool result = SUCCEEDED ( pDevice->CreatePixelShader (
				shaderBlob->GetBufferPointer ( ),
				shaderBlob->GetBufferSize ( ),
				nullptr,
				&m_pPixelShader ) );

			return result;
		}
	}

	return false;
}

void D3D11PixelShader::SetShader ( ID3D11DeviceContext* pDeviceContext )
{
	if ( pDeviceContext )
	{
		pDeviceContext->PSSetShader( m_pPixelShader.Get( ), nullptr, 0 );
	}
}

void D3D11PixelShader::SetShaderResource( ID3D11DeviceContext* pDeviceContext, UINT slot, ID3D11ShaderResourceView* pResource )
{
	if ( pDeviceContext )
	{
		pDeviceContext->PSSetShaderResources( slot, 1, &pResource );
	}
}

D3D11PixelShader::D3D11PixelShader ()
{
}

D3D11PixelShader::~D3D11PixelShader ()
{
}
