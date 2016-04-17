#include "stdafx.h"
#include "common.h"
#include "ITexture.h"
#include "ShaderResource.h"

#include <D3DX11.h>

bool CShaderResource::LoadShaderResource( ID3D11Device* pDevice, const String& fileName )
{
	HRESULT hr;
	D3DX11CreateShaderResourceViewFromFile( pDevice, fileName.c_str( ), NULL, NULL, &m_pShaderResourceView, &hr );

	if ( SUCCEEDED( hr ) )
	{
		return true;
	}

	return false;
}

ID3D11ShaderResourceView* CShaderResource::Get( ) const
{
	return m_pShaderResourceView.Get( );
}

void CShaderResource::SetShaderResourceView( Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& shaderResourceView )
{
	m_pShaderResourceView = shaderResourceView;
}

bool CShaderResource::CreateShaderResource( ID3D11Device* pDevice, const ITexture* pTexture, const D3D11_SHADER_RESOURCE_VIEW_DESC& desc )
{
	if ( pDevice == nullptr || pTexture == nullptr )
	{
		return false;
	}

	if ( SUCCEEDED( pDevice->CreateShaderResourceView( pTexture->Get( ), &desc, &m_pShaderResourceView ) ) )
	{
		return true;
	}

	return false;
}

CShaderResource::CShaderResource( ) : 
	m_pShaderResourceView( nullptr ),
	m_flag( SOURCE_RESOURCE_FLAG::TEXTURE )
{
}

CShaderResource::CShaderResource( const int srcFlag ) : 
	m_pShaderResourceView( nullptr ),
	m_flag( static_cast<SOURCE_RESOURCE_FLAG>( srcFlag ) )
{
}
