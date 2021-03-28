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
	virtual aga::VertexLayout* CreateVertexLayout( const aga::VertexShader* vs, const VERTEX_LAYOUT_TRAIT* trait, std::size_t size ) override;
	virtual aga::ComputeShader* CreateComputeShader( const void* byteCode, std::size_t byteCodeSize ) override;
	virtual aga::VertexShader* CreateVertexShader( const void* byteCode, std::size_t byteCodeSize ) override;
	virtual aga::PixelShader* CreatePixelShader( const void* byteCode, std::size_t byteCodeSize ) override;

	// RenderState
	virtual aga::BlendState* CreateBlendState( const BLEND_STATE_TRAIT& trait ) override;
	virtual aga::DepthStencilState* CreateDepthStencilState( const DEPTH_STENCIL_STATE_TRAIT& trait ) override;
	virtual aga::RasterizerState* CreateRasterizerState( const RASTERIZER_STATE_TRAIT& trait ) override;
	virtual aga::SamplerState* CreateSamplerState( const SAMPLER_STATE_TRAIT& trait ) override;

	// Viewport
	aga::Viewport* CreateViewport( int width, int height, void* hWnd, DXGI_FORMAT format );

	// UTIL
	// virtual void CopyResource( RE_HANDLE dest, const RESOURCE_REGION* destRegionOrNull, RE_HANDLE src, const RESOURCE_REGION* srcRegionOrNull ) override;
	// virtual void UpdateResourceFromMemory( RE_HANDLE dest, void* src, UINT srcRowPitch, UINT srcDepthPitch, const RESOURCE_REGION* destRegionOrNull = nullptr ) override;

	void OnDeviceLost( );
	void OnDeviceRestore( ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext );

	CD3D11ResourceManager( ) = default;
	CD3D11ResourceManager( const CD3D11ResourceManager& ) = delete;
	CD3D11ResourceManager( CD3D11ResourceManager&& ) = delete;
	CD3D11ResourceManager& operator=( const CD3D11ResourceManager& ) = delete;
	CD3D11ResourceManager& operator=( CD3D11ResourceManager&& ) = delete;

private:
	std::map<std::string, TEXTURE_TRAIT> m_textureTraits;

	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pDeviceContext;

	std::pair<int, int>	m_frameBufferSize = { 0, 0 };

	std::set<RefHandle<GraphicsApiResource>> m_renderResources;
};

