#pragma once

#include "D3D11ResourceInterface.h"
#include "IRenderResourceManager.h"
#include "RefHandle.h"

#include <map>
#include <memory>
#include <vector>

class KeyValueGroup;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11Resource;
struct ID3D11ShaderResourceView;

class CD3D11DepthStencil;
class CD3D11BlendState;
class CD3D11Buffer;
class CD3D11DepthStencilState;
class CD3D11RandomAccessResource;
class CD3D11RasterizerState;
class CD3D11SamplerState;
class CD3D11Texture1D;
class CD3D11Texture2D;
class CD3D11Texture3D;
class CD3D11RenderTarget;
class CD3D11ShaderResource;
class CD3D11ComputeShader;
class CD3D11GeometryShader;
class CD3D11PixelShader;
class CD3D11VertexLayout;
class CD3D11VertexShader;

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
	bool Bootup( ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext );
	virtual void AppSizeChanged( UINT nWndWidth, UINT nWndHeight ) override;

	// Texture
	virtual RE_HANDLE CreateTexture1D( TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData = nullptr ) override;
	virtual RE_HANDLE CreateTexture2D( TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData = nullptr ) override;
	virtual RE_HANDLE CreateTexture3D( TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData = nullptr ) override;

	// Buffer
	virtual RE_HANDLE CreateBuffer( const BUFFER_TRAIT& trait, const RESOURCE_INIT_DATA* initData = nullptr ) override;

	// Shader
	virtual RE_HANDLE CreateVertexLayout( RE_HANDLE vsHandle, const VERTEX_LAYOUT* layoutOrNull, int layoutSize ) override;
	virtual RE_HANDLE CreateVertexShader( const void* byteCodePtr, std::size_t byteCodeSize ) override;
	virtual RE_HANDLE CreateGeometryShader( const void* byteCodePtr, std::size_t byteCodeSize ) override;
	virtual RE_HANDLE CreatePixelShader( const void* byteCodePtr, std::size_t byteCodeSize ) override;
	virtual RE_HANDLE CreateComputeShader( const void* byteCodePtr, std::size_t byteCodeSize ) override;

	// RenderTarget
	virtual RE_HANDLE CreateRenderTarget( RE_HANDLE texHandle, const TEXTURE_TRAIT* trait = nullptr ) override;
	// Depth Stencil
	virtual RE_HANDLE CreateDepthStencil( RE_HANDLE texHandle, const TEXTURE_TRAIT* trait = nullptr ) override;

	// ShaderResource
	virtual RE_HANDLE CreateTexture1DShaderResource( RE_HANDLE texHandle, const TEXTURE_TRAIT* trait = nullptr ) override;
	virtual RE_HANDLE CreateTexture2DShaderResource( RE_HANDLE texHandle, const TEXTURE_TRAIT* trait = nullptr ) override;
	virtual RE_HANDLE CreateTexture3DShaderResource( RE_HANDLE texHandle, const TEXTURE_TRAIT* trait = nullptr ) override;
	virtual RE_HANDLE CreateBufferShaderResource( RE_HANDLE bufHandle, const BUFFER_TRAIT* trait = nullptr ) override;

	// RandomAccessResource
	virtual RE_HANDLE CreateTexture1DRandomAccess( RE_HANDLE texHandle, const TEXTURE_TRAIT* trait = nullptr ) override;
	virtual RE_HANDLE CreateTexture2DRandomAccess( RE_HANDLE texHandle, const TEXTURE_TRAIT* trait = nullptr ) override;
	virtual RE_HANDLE CreateBufferRandomAccess( RE_HANDLE bufHandle, const BUFFER_TRAIT* trait = nullptr ) override;

	// RenderState
	virtual RE_HANDLE CreateSamplerState( const SAMPLER_STATE_TRAIT& trait ) override;
	virtual RE_HANDLE CreateRasterizerState( const RASTERIZER_STATE_TRAIT& trait ) override;
	virtual RE_HANDLE CreateBlendState( const BLEND_STATE_TRAIT& trait ) override;
	virtual RE_HANDLE CreateDepthStencilState( const DEPTH_STENCIL_STATE_TRAIT& trait ) override;

	// UTIL
	virtual void CopyResource( RE_HANDLE dest, const RESOURCE_REGION* destRegionOrNull, RE_HANDLE src, const RESOURCE_REGION* srcRegionOrNull ) override;
	virtual void UpdateResourceFromMemory( RE_HANDLE dest, void* src, UINT srcRowPitch, UINT srcDepthPitch, const RESOURCE_REGION* destRegionOrNull = nullptr ) override;

	virtual void FreeResource( RE_HANDLE handle ) override;

	void OnDeviceLost( );
	void OnDeviceRestore( ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext );

	RE_HANDLE AddTexture2D( const Microsoft::WRL::ComPtr<ID3D11Texture2D>& texture2D, bool appSizeDepentant = false );

	CD3D11DepthStencil* GetDepthstencil( RE_HANDLE handle ) const;
	CD3D11RenderTarget* GetRendertarget( RE_HANDLE handle ) const;
	CD3D11ShaderResource* GetShaderResource( RE_HANDLE handle ) const;
	CD3D11RandomAccessResource* GetRandomAccess( RE_HANDLE handle ) const;
	CD3D11Texture1D* GetTexture1D( RE_HANDLE handle ) const;
	CD3D11Texture2D* GetTexture2D( RE_HANDLE handle ) const;
	CD3D11Texture3D* GetTexture3D( RE_HANDLE handle ) const;
	CD3D11Buffer* GetBuffer( RE_HANDLE handle ) const;
	CD3D11VertexLayout* GetVertexLayout( RE_HANDLE handle );
	CD3D11VertexShader* GetVertexShader( RE_HANDLE handle ) const;
	CD3D11GeometryShader* GetGeometryShader( RE_HANDLE handle ) const;
	CD3D11PixelShader* GetPixelShader( RE_HANDLE handle ) const;
	CD3D11ComputeShader* GetComputeShader( RE_HANDLE handle ) const;
	CD3D11SamplerState* GetSamplerState( RE_HANDLE handle ) const;
	CD3D11RasterizerState* GetRasterizerState( RE_HANDLE handle ) const;
	CD3D11BlendState* GetBlendState( RE_HANDLE handle ) const;
	CD3D11DepthStencilState* GetDepthStencilState( RE_HANDLE handle ) const;

	ID3D11Resource* GetD3D11ResourceGeneric( RE_HANDLE handle );

	IDeviceDependant* GetGraphicsResource( RE_HANDLE handle ) const;

	CD3D11ResourceManager( ) = default;
	CD3D11ResourceManager( const CD3D11ResourceManager& ) = delete;
	CD3D11ResourceManager( CD3D11ResourceManager&& ) = delete;
	CD3D11ResourceManager& operator=( const CD3D11ResourceManager& ) = delete;
	CD3D11ResourceManager& operator=( CD3D11ResourceManager&& ) = delete;

private:
	std::vector<RefHandle<CD3D11DepthStencil>> m_depthStencils;
	std::vector<RefHandle<CD3D11RenderTarget>> m_renderTargets;
	std::vector<RefHandle<CD3D11ShaderResource>> m_shaderResources;
	std::vector<RefHandle<CD3D11RandomAccessResource>> m_randomAccessResources;
	std::vector<RefHandle<CD3D11Texture1D>> m_texture1Ds;
	std::vector<RefHandle<CD3D11Texture2D>> m_texture2Ds;
	std::vector<RefHandle<CD3D11Texture3D>> m_texture3Ds;
	std::vector<RefHandle<CD3D11Buffer>> m_buffers;
	std::vector<RefHandle<CD3D11VertexLayout>> m_vertexLayouts;
	std::vector<RefHandle<CD3D11VertexShader>> m_vertexShaders;
	std::vector<RefHandle<CD3D11GeometryShader>> m_geometryShaders;
	std::vector<RefHandle<CD3D11PixelShader>> m_pixelShaders;
	std::vector<RefHandle<CD3D11ComputeShader>> m_computeShaders;
	std::vector<RefHandle<CD3D11SamplerState>> m_samplerStates;
	std::vector<RefHandle<CD3D11RasterizerState>> m_rasterizerStates;
	std::vector<RefHandle<CD3D11BlendState>> m_blendStates;
	std::vector<RefHandle<CD3D11DepthStencilState>> m_depthStencilStates;

	std::map<String, TEXTURE_TRAIT> m_textureTraits;

	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pDeviceContext;

	std::pair<int, int>	m_frameBufferSize = { 0, 0 };
};

