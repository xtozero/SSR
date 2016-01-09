#pragma once

#include "common.h"
#include <map>
#include <memory>

class ITexture;
struct ID3D11Device;

class CTextureManager
{
public:
	void LoadTextureFromFile( ID3D11Device* pDevice, const String& fileName );
	std::shared_ptr<ITexture> GetTexture( const String& fileName );

	CTextureManager( );
	~CTextureManager( );

private:
	std::map<String, std::shared_ptr<ITexture>> m_textures;
};

