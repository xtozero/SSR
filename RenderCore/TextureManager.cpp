#include "stdafx.h"

#include "Texture2D.h"
#include "TextureManager.h"

#include "../shared/Util.h"

#include <D3DX11.h>

namespace
{
	const TCHAR* DEFAULT_TEXTURE_FILE_PATH = _T( "../Texture/" );
}

bool CTextureManager::LoadTextureFromFile( ID3D11Device* pDevice, const String& fileName )
{
	if ( pDevice == nullptr )
	{
		return false;
	}

	TCHAR pPath[MAX_PATH];
	::GetCurrentDirectory( MAX_PATH, pPath );
	::SetCurrentDirectory( DEFAULT_TEXTURE_FILE_PATH );

	D3DX11_IMAGE_INFO info;
	HRESULT hr;
	D3DX11GetImageInfoFromFile( fileName.c_str( ), NULL, &info, &hr );

	std::shared_ptr<ITexture> newTexture = nullptr;

	if ( SUCCEEDED( hr ) )
	{
		switch ( info.ResourceDimension )
		{
		case D3D11_RESOURCE_DIMENSION_TEXTURE1D:
			break;
		case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
			newTexture = std::make_shared<CTexture2D>( );
			break;
		case D3D11_RESOURCE_DIMENSION_TEXTURE3D:
			break;
		}

		if ( newTexture && newTexture->LoadFromFile( pDevice, fileName ) )
		{
			m_pTextures.emplace( fileName, newTexture );
			::SetCurrentDirectory( pPath );
			return true;
		}
	}

	::SetCurrentDirectory( pPath );
	return false;
}

bool CTextureManager::CreateTexture2D( ID3D11Device* pDevice, const D3D11_TEXTURE2D_DESC& desc, const String& textureName )
{
	if ( FindTexture( textureName ) )
	{
		DebugWarning( "CTextureManager Error - Try Create Exist Texture Name\n" );
		return false;
	}

	if ( pDevice )
	{
		std::shared_ptr<ITexture> newTexture = std::make_shared<CTexture2D>();

		if ( newTexture && newTexture->Create( pDevice, desc ) )
		{
			m_pTextures.emplace( textureName, newTexture );
			return true;
		}
	}

	return false;
}

std::shared_ptr<ITexture> CTextureManager::FindTexture( const String& textureName ) const
{
	auto found = m_pTextures.find( textureName );

	if ( found != m_pTextures.end( ) )
	{
		return found->second;
	}

	return nullptr;
}

CTextureManager::CTextureManager( )
{
}


CTextureManager::~CTextureManager( )
{
}
