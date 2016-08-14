#pragma once

#include "common.h"

#include <memory>

class CShaderResourceManager;
class CTextureManager;
class TextureDescription;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11Texture2D;
class IShaderResource;
class ITexture;

class CSnapshotManager
{
public:
	bool TakeSnapshot2D( ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const String& sourceTextureName, const String& destTextureName );
	ITexture* CreateCloneTexture( ID3D11Device* pDevice, const ITexture* pSourceTexture, const String& textureName );
	IShaderResource* TryCreateShaderResource( ID3D11Device* pDevice, const ITexture* pTexture, const TextureDescription& desc, const String& textureName, int srcFlag = 0 );

	CSnapshotManager( CTextureManager* pTextureManager, CShaderResourceManager* pShaderResourceManager );
	~CSnapshotManager( ) = default;

private:
	CTextureManager* m_pTextureMgr;
	CShaderResourceManager* m_pShaderResourceMgr;
};

