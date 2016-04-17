#pragma once

#include "common.h"
#include "ITexture.h"
#include "../Engine/ScriptKeyHandler.h"

#include <map>
#include <memory>

struct D3D11_TEXTURE2D_DESC;
class KeyValueGroup;

namespace Microsoft
{
	namespace WRL
	{
		template <typename T> class ComPtr;
	}
}

class CTextureManager : public CScriptKeyHandler<CTextureManager>
{
public:
	bool LoadTexture( ID3D11Device* pDevice );
	bool LoadTextureFromFile( ID3D11Device* pDevice, const String& fileName );
	bool LoadTextureFromScript( ID3D11Device* pDevice, const String& fileName );

	ITexture* CreateTexture2D( ID3D11Device* pDevice, const D3D11_TEXTURE2D_DESC& desc, const String& textureName, const D3D11_SUBRESOURCE_DATA* pInitialData = nullptr );
	ITexture* CreateTexture2D( ID3D11Device* pDevice, const String descName, const String& textureName, const D3D11_SUBRESOURCE_DATA* pInitialData = nullptr );
	bool RegisterTexture2D( const String& textureName, Microsoft::WRL::ComPtr<ID3D11Resource> pTexture );
	ITexture* FindTexture( const String& textureName ) const;

	CTextureManager( );
	~CTextureManager( ) = default;

private:
	bool LoadTextureFromScriptInternal( ID3D11Device* pDevice, const std::shared_ptr<KeyValueGroup>& keyValue );

	std::map<String, std::shared_ptr<ITexture>> m_pTextures;
	std::map<String, D3D11_TEXTURE2D_DESC> m_texture2DDesc;
};