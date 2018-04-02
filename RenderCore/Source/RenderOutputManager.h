#pragma once

#include "Render/Resource.h"

#include <array>

struct DXGI_SAMPLE_DESC;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
class IRenderer;
class IRenderResource;
class IResourceManager;

enum RENDER_OUPUT_TYPE
{
	FRAME_BUFFER = 0,
	NORMAL_BUFFER,
	DEPTH_BUFFER,
	COUNT
};

class CRenderOutputManager
{
public:
	bool Initialize( IResourceManager& resourceMgr, IDXGISwapChain& pSwapChain, DXGI_SAMPLE_DESC& sampleDesc );
	void AppSizeChanged( IResourceManager& resourceMgr, IDXGISwapChain& pSwapChain, DXGI_SAMPLE_DESC& sampleDesc );
	void SetRenderTargetDepthStencilView( IRenderer& renderer );
	void ClearDepthStencil( IRenderer& renderer );
	void ClearRenderTargets( IRenderer& renderer, const float( &clearColor )[4] );
	void SceneEnd( IRenderer& deviceContext );

	CRenderOutputManager( );
private:
	
	bool CreateDefaultRenderTaraget( IResourceManager& resourceMgr, IDXGISwapChain& pSwapChain );
	bool CreateNormalRenderTarget( IResourceManager& resourceMgr, DXGI_SAMPLE_DESC& sampleDesc );
	bool CreateDepthRenderTarget( IResourceManager& resourceMgr, DXGI_SAMPLE_DESC& sampleDesc );
	bool CreateDefaultDepthStencil( IResourceManager& resourceMgr, DXGI_SAMPLE_DESC& sampleDesc );

	std::array<RE_HANDLE, RENDER_OUPUT_TYPE::COUNT>	m_renderSRVs;
	std::array<RE_HANDLE, RENDER_OUPUT_TYPE::COUNT>	m_renderOutputs;
	RE_HANDLE										m_primeDs = RE_HANDLE_TYPE::INVALID_HANDLE;
};

