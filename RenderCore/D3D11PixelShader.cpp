#include "stdafx.h"

#include "common.h"
#include "D3D11PixelShader.h"
#include "IShaderResource.h"

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

void D3D11PixelShader::SetShaderResource( ID3D11DeviceContext* pDeviceContext, UINT slot, std::shared_ptr<IShaderResource>& pResource )
{
	if ( pDeviceContext )
	{
		ID3D11ShaderResourceView* srv = nullptr;

		if ( pResource.get() && pResource->Get( ) )
		{
			srv = pResource->Get( );
		}

		pDeviceContext->PSSetShaderResources( slot, 1, &srv );
	}
}

D3D11PixelShader::D3D11PixelShader ()
{
}

D3D11PixelShader::~D3D11PixelShader ()
{
}
