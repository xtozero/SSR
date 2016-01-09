#include "stdafx.h"
#include <d3d11.h>
#include <fstream>
#include "ITexture.h"
#include "Texture.h"
#include "TextureManager.h"
#include "../shared/Util.h"

namespace
{
	const TCHAR* DEFAULT_TEXTURE_FILE_PATH = _T( "../Texture/" );
}

void CTextureManager::LoadTextureFromFile( ID3D11Device* pDevice, const String& fileName )
{
	TCHAR pPath[MAX_PATH];
	::GetCurrentDirectory( MAX_PATH, pPath );
	::SetCurrentDirectory( DEFAULT_TEXTURE_FILE_PATH );

	Ifstream textureFile;
	textureFile.open( fileName, 0 );

	if ( !textureFile.is_open( ) )
	{
		::SetCurrentDirectory( pPath );
		return;
	}

	std::shared_ptr<ITexture> newTexture = std::make_shared<CTexture>();
	if ( newTexture && newTexture->LoadTexture( pDevice, fileName ) )
	{
		m_textures.emplace( fileName.c_str( ), newTexture );
	}

	textureFile.close( );

	::SetCurrentDirectory( pPath );
}

std::shared_ptr<ITexture> CTextureManager::GetTexture( const String& fileName )
{
	auto found = m_textures.find( fileName );

	if ( found != m_textures.end( ) )
	{
		return found->second;
	}
	else
	{
		return nullptr;
	}
}

CTextureManager::CTextureManager( )
{
}


CTextureManager::~CTextureManager( )
{
}
