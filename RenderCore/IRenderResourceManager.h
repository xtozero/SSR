#pragma once

#include "common.h"

struct D3D11_DEPTH_STENCIL_VIEW_DESC;
struct D3D11_RENDER_TARGET_VIEW_DESC;
struct D3D11_SUBRESOURCE_DATA;
struct D3D11_TEXTURE2D_DESC;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11Resource;
struct ID3D11ShaderResourceView;

class CShaderResourceViewDescription;
class IRenderTarget;
class IDepthStencil;
class IShaderResource;
class ITexture;
class RenderTargetBinder;
class TextureDescription;

namespace Microsoft
{
	namespace WRL
	{
		template <typename T> class ComPtr;
	}
}

class IRenderTargetManager
{
public:
	virtual IRenderTarget* CreateRenderTarget( ID3D11Device* pDevice, const ITexture* pTexture, const D3D11_RENDER_TARGET_VIEW_DESC* rtvDesc, const String& renderTargetName ) = 0;
	virtual IDepthStencil* CreateDepthStencil( ID3D11Device* pDevice, const ITexture* pTexture, const D3D11_DEPTH_STENCIL_VIEW_DESC* dsvDesc, const String& depthStencilName ) = 0;
	virtual IRenderTarget* FindRenderTarget( const String& renderTargetName ) const = 0;
	virtual IDepthStencil* FindDepthStencil( const String& depthStencilName ) const = 0;

	virtual void SetRenderTarget( ID3D11DeviceContext* pDeviceContext, IRenderTarget* pRenderTarget, IDepthStencil* pDepthStencil ) = 0;
	virtual void SetRenderTarget( ID3D11DeviceContext* pDeviceContext, RenderTargetBinder& pBinder, IDepthStencil* pDepthStencil ) = 0;
};

class ITextureManager
{
public:
	virtual bool LoadTexture( ID3D11Device* pDevice ) = 0;
	virtual bool LoadTextureFromFile( ID3D11Device* pDevice, const String& fileName ) = 0;
	virtual bool LoadTextureFromScript( ID3D11Device* pDevice, const String& fileName ) = 0;

	virtual ITexture* CreateTexture2D( ID3D11Device* pDevice, const TextureDescription& desc, const String& textureName, const D3D11_SUBRESOURCE_DATA* pInitialData = nullptr ) = 0;
	virtual ITexture* CreateTexture2D( ID3D11Device* pDevice, const String& descName, const String& textureName, const D3D11_SUBRESOURCE_DATA* pInitialData = nullptr ) = 0;
	virtual bool RegisterTexture2D( const String& textureName, Microsoft::WRL::ComPtr<ID3D11Resource> pTexture ) = 0;
	virtual ITexture* FindTexture( const String& textureName ) const = 0;

	virtual void SetFrameBufferSize( UINT nWndWidth, UINT nWndHeight ) = 0;
	virtual const std::pair<int, int>& GetFrameBufferSize( ) const = 0;
};

class IShaderResourceManager
{
public:
	virtual void LoadShaderResourceFromFile( ID3D11Device* pDevice, const String& fileName ) = 0;
	virtual IShaderResource* FindShaderResource( const String& fileName ) const = 0;
	virtual void RegisterShaderResource( const String& resourceName, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& srView, int srcFlag = 0 ) = 0;
	virtual IShaderResource* CreateShaderResource( ID3D11Device* pDevice, const ITexture* pTexture, const CShaderResourceViewDescription* desc, const String& resourceName, int srcFlag = 0 ) = 0;
};

class ISnapshotManager
{
public:
	virtual bool TakeSnapshot2D( ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const String& sourceTextureName, const String& destTextureName ) = 0;
	virtual ITexture* CreateCloneTexture( ID3D11Device* pDevice, const ITexture* pSourceTexture, const String& textureName ) = 0;
	virtual IShaderResource* TryCreateShaderResource( ID3D11Device* pDevice, const ITexture* pTexture, const TextureDescription& desc, const String& textureName, int srcFlag = 0 ) = 0;
};

