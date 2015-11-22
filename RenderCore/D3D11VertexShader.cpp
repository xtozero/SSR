#include "stdafx.h"

#include "common.h"
#include "D3D11VertexShader.h"

bool D3D11VertexShader::CreateShader ( ID3D11Device* pDevice, const TCHAR* pFilePath, const char* pProfile )
{
	if ( pDevice && m_pInputElementDesc )
	{
		ID3D10Blob* shaderBlob = GetShaderBlob( pFilePath, pProfile ) ;

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

			SAFE_RELEASE ( shaderBlob );
			return result;
		}
	}

	return false;
}

void D3D11VertexShader::SetShader ( ID3D11DeviceContext* pDeviceContext )
{
	if ( pDeviceContext )
	{
		pDeviceContext->VSSetShader( m_pVertexShader, nullptr, 0 );
		pDeviceContext->IASetInputLayout ( m_pInputLayout );
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
	SAFE_RELEASE ( m_pVertexShader );
	SAFE_ARRAY_DELETE ( m_pInputElementDesc );
	SAFE_RELEASE ( m_pInputLayout );
}