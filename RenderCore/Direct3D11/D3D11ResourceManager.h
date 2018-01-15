#pragma once

#include "../CommonRenderer/IRenderResourceManager.h"

#include <map>
#include <memory>

class KeyValueGroup;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11ShaderResourceView;

namespace Microsoft
{
	namespace WRL
	{
		template <typename T> class ComPtr;
	}
}

class CD3D11ResourceManager : public IResourceManager
{
public:
	virtual bool Bootup( ) override;

	// Texture
	virtual bool LoadTextureFromFile( const String& fileName ) override;

	virtual ITexture* CreateTexture1D( const TEXTURE_TRAIT& trait, const String& textureName, const RESOURCE_INIT_DATA* initData = nullptr ) override;
	virtual ITexture* CreateTexture1D( const String& descName, const String& textureName, const RESOURCE_INIT_DATA* initData = nullptr ) override;

	virtual ITexture* CreateTexture2D( const TEXTURE_TRAIT& trait, const String& textureName, const RESOURCE_INIT_DATA* initData = nullptr ) override;
	virtual ITexture* CreateTexture2D( const String& descName, const String& textureName, const RESOURCE_INIT_DATA* initData = nullptr ) override;

	virtual ITexture* RegisterTexture2D( const String& textureName, void* pTexture, bool isAppSizeDependent = false ) override;
	virtual ITexture* FindTexture( const String& textureName ) const override;

	virtual void AppSizeChanged( UINT nWndWidth, UINT nWndHeight ) override;

	// RenderTarget
	virtual IRenderResource* CreateRenderTarget( const ITexture& texture, const String& renderTargetName, const TEXTURE_TRAIT* trait = nullptr ) override;
	virtual IRenderResource* CreateDepthStencil( const ITexture& texture, const String& depthStencilName, const TEXTURE_TRAIT* trait = nullptr ) override;
	virtual IRenderResource* FindRenderTarget( const String& renderTargetName ) const override;
	virtual IRenderResource* FindDepthStencil( const String& depthStencilName ) const override;

	// ShaderResource
	virtual void LoadShaderResourceFromFile( const String& fileName ) override;
	virtual IRenderResource* FindShaderResource( const String& fileName ) const override;
	virtual IRenderResource* CreateShaderResource( const ITexture& texture, const String& resourceName, const TEXTURE_TRAIT* trait = nullptr ) override;
	virtual IRenderResource* CreateShaderResource( const IBuffer& buffer, const String& resourceName, const BUFFER_TRAIT* trait = nullptr ) override;

	void RegisterShaderResource( const String& resourceName, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& srView );

	// RandomAccessResource
	virtual IRenderResource* FindRandomAccessResource( const String& resourceName ) const override;
	virtual IRenderResource* CreateRandomAccessResource( const ITexture& texture, const String& resourceName, const TEXTURE_TRAIT* trait = nullptr ) override;
	virtual IRenderResource* CreateRandomAccessResource( const IBuffer& buffer, const String& resourceName, const BUFFER_TRAIT* trait = nullptr ) override;

	// UTIL
	virtual ITexture* CreateCloneTexture( const ITexture& src, const String& textureName ) override;
	virtual void CopyResource( IRenderResource& dest, const RESOURCE_REGION* destRegionOrNull, IRenderResource& src, const RESOURCE_REGION* srcRegionOrNull ) override;

	void TakeSnapshot( const String& sourceTextureName, const String& destTextureName );

	CD3D11ResourceManager( ID3D11Device& device, ID3D11DeviceContext& deviceContext );

private:
	std::map<String, std::unique_ptr<IRenderResource>> m_depthStencils;
	std::map<String, std::unique_ptr<IRenderResource>> m_renderTargets;
	std::map<String, std::unique_ptr<IRenderResource>> m_shaderResources;
	std::map<String, std::unique_ptr<IRenderResource>> m_randomAccessResource;
	std::map<String, std::unique_ptr<ITexture>> m_pTextures;

	std::map<String, TEXTURE_TRAIT> m_textureTraits;

	ID3D11Device& m_device;
	ID3D11DeviceContext& m_deviceContext;

	std::pair<int, int>	m_frameBufferSize;
};

