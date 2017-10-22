#pragma once

#include "../common.h"

struct RESOURCE_INIT_DATA;
struct TEXTURE_TRAIT;

class IRenderResource;
class ITexture;

namespace Microsoft
{
	namespace WRL
	{
		template <typename T> class ComPtr;
	}
}

class IResourceManager
{
public:
	virtual bool Bootup( ) = 0;
	virtual bool LoadTextureFromFile( const String& fileName ) = 0;

	virtual ITexture* CreateTexture1D( const TEXTURE_TRAIT& trait, const String& textureName, const RESOURCE_INIT_DATA* initData = nullptr ) = 0;
	virtual ITexture* CreateTexture1D( const String& descName, const String& textureName, const RESOURCE_INIT_DATA* initData = nullptr ) = 0;

	virtual ITexture* CreateTexture2D( const TEXTURE_TRAIT& trait, const String& textureName, const RESOURCE_INIT_DATA* initData = nullptr ) = 0;
	virtual ITexture* CreateTexture2D( const String& descName, const String& textureName, const RESOURCE_INIT_DATA* initData = nullptr ) = 0;

	virtual ITexture* RegisterTexture2D( const String& textureName, void* pTexture ) = 0;
	virtual ITexture* FindTexture( const String& textureName ) const = 0;

	virtual void SetFrameBufferSize( UINT nWndWidth, UINT nWndHeight ) = 0;

	virtual IRenderResource* CreateRenderTarget( const ITexture& texture, const String& renderTargetName, const TEXTURE_TRAIT* trait = nullptr ) = 0;
	virtual IRenderResource* CreateDepthStencil( const ITexture& texture, const String& depthStencilName, const TEXTURE_TRAIT* trait = nullptr ) = 0;
	virtual IRenderResource* FindRenderTarget( const String& renderTargetName ) const = 0;
	virtual IRenderResource* FindDepthStencil( const String& depthStencilName ) const = 0;

	virtual void LoadShaderResourceFromFile( const String& fileName ) = 0;
	virtual IRenderResource* FindShaderResource( const String& fileName ) const = 0;
	virtual IRenderResource* CreateShaderResource( const ITexture& texture, const String& resourceName, const TEXTURE_TRAIT* trait = nullptr ) = 0;

	virtual ITexture* CreateCloneTexture( const ITexture& pSourceTexture, const String& textureName ) = 0;
};