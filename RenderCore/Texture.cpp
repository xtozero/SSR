#include "stdafx.h"
#include "Texture.h"

#include <D3D11.h>
#include <D3DX11.h>
#include <wrl/client.h>

bool CTexture2D::LoadFromFile( ID3D11Device* pDevice, const String& fileName )
{
	if ( pDevice )
	{
		HRESULT hr;
		ID3D11Resource* pNewTexture;

		D3DX11CreateTextureFromFile( pDevice, fileName.c_str( ), nullptr, nullptr, &pNewTexture, &hr );

		if ( SUCCEEDED( hr ) )
		{
			if ( SUCCEEDED( pNewTexture->QueryInterface( IID_ID3D11Texture2D, (void **)&m_pTexture2D ) ) )
			{
				return true;
			}
		}
	}

	return false;
}

ID3D11Resource* CTexture2D::Get( ) const
{
	if ( m_pTexture2D.Get( ) )
	{
		ID3D11Resource* pResource;
		if ( SUCCEEDED( m_pTexture2D.Get()->QueryInterface( IID_ID3D11Resource, (void**)&pResource ) ) )
		{
			return pResource;
		}
	}

	return nullptr;
}

bool CTexture2D::SetTexture( Microsoft::WRL::ComPtr<ID3D11Resource>& pTexture )
{
	if ( SUCCEEDED( pTexture.Get()->QueryInterface( IID_ID3D11Texture2D, (void **)&m_pTexture2D ) ) )
	{
		return true;
	}

	return false;
}

bool CTexture2D::Create( ID3D11Device* pDevice, const D3D11_TEXTURE2D_DESC& desc, const D3D11_SUBRESOURCE_DATA* pInitialData )
{
	if ( pDevice )
	{
		if ( SUCCEEDED( pDevice->CreateTexture2D( &desc, pInitialData, &m_pTexture2D ) ) )
		{
			return true;
		}
	}

	return false;
}

CTexture2D::CTexture2D( ) : m_pTexture2D( nullptr )
{
}