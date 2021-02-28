#pragma once

#include "D3D11ResourceInterface.h"
#include "D3D11VetexLayout.h"
#include "IRenderResourceManager.h"
#include "RefHandle.h"

#include <map>
#include <memory>
#include <set>
#include <vector>

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11Resource;
struct ID3D11ShaderResourceView;

class CD3D11BlendState;
class D3D11DepthStencilState;
class CD3D11RasterizerState;
class CD3D11SamplerState;

namespace aga
{
	class Viewport;
}

namespace Microsoft
{
	namespace WRL
	{
		template <typename T> class ComPtr;
	}
}

class CD3D11ResourceManager final : public IResourceManager
{
public:
	bool Bootup( ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext );
	virtual void AppSizeChanged( UINT nWndWidth, UINT nWndHeight ) override;

	// Texture
	virtual aga::Texture* CreateTexture( const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData = nullptr ) override;

	// Buffer
	virtual aga::Buffer* CreateBuffer( const BUFFER_TRAIT& trait, const void* initData = nullptr ) override;

	// Shader
	virtual VertexLayout* FindAndCreateVertexLayout( const aga::VertexShader* vs, const VertexLayoutDesc& layoutDesc ) override;
	virtual aga::ComputeShader* CreateComputeShader( const void* byteCode, std::size_t byteCodeSize ) override;
	virtual aga::VertexShader* CreateVertexShader( const void* byteCode, std::size_t byteCodeSize ) override;
	virtual aga::PixelShader* CreatePixelShader( const void* byteCode, std::size_t byteCodeSize ) override;

	// RenderState
	virtual RE_HANDLE CreateSamplerState( const SAMPLER_STATE_TRAIT& trait ) override;
	virtual RE_HANDLE CreateRasterizerState( const RASTERIZER_STATE_TRAIT& trait ) override;
	virtual RE_HANDLE CreateBlendState( const BLEND_STATE_TRAIT& trait ) override;
	virtual aga::DepthStencilState* CreateDepthStencilState( const DEPTH_STENCIL_STATE_TRAIT& trait ) override;

	// Viewport
	aga::Viewport* CreateViewport( int width, int height, void* hWnd, DXGI_FORMAT format );

	// UTIL
	virtual void CopyResource( RE_HANDLE dest, const RESOURCE_REGION* destRegionOrNull, RE_HANDLE src, const RESOURCE_REGION* srcRegionOrNull ) override;
	virtual void UpdateResourceFromMemory( RE_HANDLE dest, void* src, UINT srcRowPitch, UINT srcDepthPitch, const RESOURCE_REGION* destRegionOrNull = nullptr ) override;

	virtual void FreeResource( RE_HANDLE handle ) override;

	void OnDeviceLost( );
	void OnDeviceRestore( ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext );

	CD3D11SamplerState* GetSamplerState( RE_HANDLE handle ) const;
	CD3D11RasterizerState* GetRasterizerState( RE_HANDLE handle ) const;
	CD3D11BlendState* GetBlendState( RE_HANDLE handle ) const;

	IDeviceDependant* GetGraphicsResource( RE_HANDLE handle ) const;

	CD3D11ResourceManager( ) = default;
	CD3D11ResourceManager( const CD3D11ResourceManager& ) = delete;
	CD3D11ResourceManager( CD3D11ResourceManager&& ) = delete;
	CD3D11ResourceManager& operator=( const CD3D11ResourceManager& ) = delete;
	CD3D11ResourceManager& operator=( CD3D11ResourceManager&& ) = delete;

private:
	std::vector<RefHandle<CD3D11SamplerState>> m_samplerStates;
	std::vector<RefHandle<CD3D11RasterizerState>> m_rasterizerStates;
	std::vector<RefHandle<CD3D11BlendState>> m_blendStates;

	std::map<std::string, TEXTURE_TRAIT> m_textureTraits;

	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pDeviceContext;

	std::pair<int, int>	m_frameBufferSize = { 0, 0 };

	std::set<RefHandle<GraphicsApiResource>> m_renderResources;
	std::map<VertexLayoutDesc, RefHandle<D3D11VertexLayout>> m_vertexLayouts;
};

