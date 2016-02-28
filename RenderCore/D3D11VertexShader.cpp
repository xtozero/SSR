#include "stdafx.h"

#include "common.h"
#include "D3D11VertexShader.h"
#include "util_rendercore.h"

#include <D3D11.h>

bool D3D11VertexShader::CreateShader ( ID3D11Device* pDevice, const TCHAR* pFilePath, const char* pProfile )
{
	if ( pDevice && m_pInputElementDesc )
	{
		Microsoft::WRL::ComPtr<ID3D10Blob> shaderBlob = GetShaderBlob( pFilePath, pProfile );

		if ( shaderBlob )
		{
			bool result = SUCCEEDED ( pDevice->CreateInputLayout ( m_pInputElementDesc,
				m_numInputElement,
				shaderBlob->GetBufferPointer ( ),
				shaderBlob->GetBufferSize ( ),
				&m_pInputLayout ) );

			if ( result )
			{
				result = SUCCEEDED ( pDevice->CreateVertexShader (
					shaderBlob->GetBufferPointer ( ),
					shaderBlob->GetBufferSize ( ),
					nullptr,
					&m_pVertexShader ) );
			}

#ifdef _DEBUG
			SetDebugName( m_pVertexShader.Get( ), "Vertex Shader" );
#endif

			return result;
		}
	}

	return false;
}

void D3D11VertexShader::SetShader ( ID3D11DeviceContext* pDeviceContext )
{
	if ( pDeviceContext )
	{
		pDeviceContext->VSSetShader( m_pVertexShader.Get( ), nullptr, 0 );
		pDeviceContext->IASetInputLayout( m_pInputLayout.Get( ) );
	}
}

void D3D11VertexShader::SetShaderResource( ID3D11DeviceContext* pDeviceContext, UINT slot, ID3D11ShaderResourceView* pResource )
{
	if ( pDeviceContext )
	{
		pDeviceContext->VSSetShaderResources( slot, 1, &pResource );
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