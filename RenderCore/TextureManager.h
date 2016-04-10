#pragma once

#include "common.h"
#include "ITexture.h"

#include <map>
#include <memory>

struct D3D11_TEXTURE2D_DESC;

class CTextureManager
{
public:
	bool LoadTextureFromFile( ID3D11Device* pDevice, const String& fileName );
	bool CreateTexture2D( ID3D11Device* pDevice, const D3D11_TEXTURE2D_DESC& desc, const String& textureName );
	std::shared_ptr<ITexture> FindTexture( const String& textureName ) const;

	CTextureManager( );
	~CTextureManager( );

private:
	std::map<String, std::shared_ptr<ITexture>> m_pTextures;
};

