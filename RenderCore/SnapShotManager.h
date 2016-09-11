#pragma once

#include "common.h"
#include "IRenderResourceManager.h"

#include <memory>

class CSnapshotManager : public ISnapshotManager
{
public:
	virtual bool TakeSnapshot2D( ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const String& sourceTextureName, const String& destTextureName ) override;
	virtual ITexture* CreateCloneTexture( ID3D11Device* pDevice, const ITexture* pSourceTexture, const String& textureName ) override;
	virtual IShaderResource* TryCreateShaderResource( ID3D11Device* pDevice, const ITexture* pTexture, const TextureDescription& desc, const String& textureName, int srcFlag = 0 ) override;

	CSnapshotManager( ITextureManager* pTextureManager, IShaderResourceManager* pShaderResourceManager );
	~CSnapshotManager( ) = default;

private:
	ITextureManager* m_pTextureMgr;
	IShaderResourceManager* m_pShaderResourceMgr;
};

