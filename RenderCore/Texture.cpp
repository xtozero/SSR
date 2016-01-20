#include "stdafx.h"
#include "common.h"
#include <D3DX11.h>
#include "Texture.h"

bool CTexture::LoadTexture( ID3D11Device* pDevice, const String& fileName )
{
	HRESULT hr;
	D3DX11CreateShaderResourceViewFromFile( pDevice, fileName.c_str( ), NULL, NULL, &m_pTextureView, &hr );

	if ( SUCCEEDED( hr ) )
	{
		return true;
	}

	return false;
}

ID3D11ShaderResourceView* CTexture::GetResource( )
{
	return m_pTextureView.Get( );
}

CTexture::CTexture( ) : m_pTextureView( nullptr )
{
}


CTexture::~CTexture( )
{
}
