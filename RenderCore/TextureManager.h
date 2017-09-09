#pragma once

#include "common.h"
#include "IRenderResourceManager.h"
#include "TextureDescription.h"
#include "../Engine/ScriptKeyHandler.h"

#include <map>
#include <memory>

class KeyValueGroup;

class CTextureManager : public ITextureManager, public CScriptKeyHandler<CTextureManager>
{
public:
	bool LoadTexture( ID3D11Device* pDevice );
	bool LoadTextureFromFile( ID3D11Device* pDevice, const String& fileName );
	bool LoadTextureFromScript( ID3D11Device* pDevice, const String& fileName );

	virtual ITexture* CreateTexture2D( ID3D11Device* pDevice, const TextureDescription& desc, const String& textureName, const D3D11_SUBRESOURCE_DATA* pInitialData = nullptr );
	virtual ITexture* CreateTexture2D( ID3D11Device* pDevice, const String& descName, const String& textureName, const D3D11_SUBRESOURCE_DATA* pInitialData = nullptr );
	bool RegisterTexture2D( const String& textureName, Microsoft::WRL::ComPtr<ID3D11Resource> pTexture );
	ITexture* FindTexture( const String& textureName ) const;

	void SetFrameBufferSize( UINT nWndWidth, UINT nWndHeight );
	const std::pair<int, int>& GetFrameBufferSize( ) const { return m_frameBufferSize; }

	CTextureManager( );
	~CTextureManager( ) = default;

private:
	bool LoadTextureFromScriptInternal( ID3D11Device* pDevice, const KeyValueGroup* keyValue );

	std::map<String, std::unique_ptr<ITexture>> m_pTextures;
	std::map<String, TextureDescription> m_texture2DDesc;
	std::pair<int, int>	m_frameBufferSize;
};