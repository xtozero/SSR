#pragma once

#include <array>
#include <tuple>

struct ID3D11DeviceContext;
struct IDXGISwapChain;
class IBuffer;
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

using rtClearColor = std::tuple<float, float, float, float>;

class CRenderOutputManager
{
public:
	bool Initialize( IResourceManager& resourceMgr, IDXGISwapChain& pSwapChain );
	void AppSizeChanged( IResourceManager& resourceMgr, IDXGISwapChain& pSwapChain );
	void SetRenderTargetDepthStencilView( IRenderer& renderer );
	void ClearDepthStencil( IRenderer& renderer );
	void ClearRenderTargets( IRenderer& renderer, const rtClearColor& clearColor );
	void SceneEnd( ID3D11DeviceContext& deviceContext );

	CRenderOutputManager( );
private:
	
	bool CreateDefaultRenderTaraget( IResourceManager& resourceMgr, IDXGISwapChain& pSwapChain );
	bool CreateNormalRenderTarget( IResourceManager& resourceMgr );
	bool CreateDepthRenderTarget( IResourceManager& resourceMgr );
	bool CreateDefaultDepthStencil( IResourceManager& resourceMgr );

	std::array<IRenderResource*, RENDER_OUPUT_TYPE::COUNT>	m_renderSRVs;
	std::array<IRenderResource*, RENDER_OUPUT_TYPE::COUNT>	m_renderOutputs;
	IRenderResource*										m_pPrimeDs;
};

