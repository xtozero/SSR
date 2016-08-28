#pragma once

#include "common.h"

#include <memory>

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11Texture2D;
class IShaderResource;
class IShaderResourceManager;
class ITexture;
class ITextureManager;
class TextureDescription;

class CSnapshotManager
{
public:
	bool TakeSnapshot2D( ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const String& sourceTextureName, const String& destTextureName );
	ITexture* CreateCloneTexture( ID3D11Device* pDevice, const ITexture* pSourceTexture, const String& textureName );
	IShaderResource* TryCreateShaderResource( ID3D11Device* pDevice, const ITexture* pTexture, const TextureDescription& desc, const String& textureName, int srcFlag = 0 );

	CSnapshotManager( ITextureManager* pTextureManager, IShaderResourceManager* pShaderResourceManager );
	~CSnapshotManager( ) = default;

private:
	ITextureManager* m_pTextureMgr;
	IShaderResourceManager* m_pShaderResourceMgr;
};

