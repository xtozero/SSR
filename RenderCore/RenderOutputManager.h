#pragma once

#include <array>
#include <tuple>

struct IDXGISwapChain;
struct ID3D11Device;
struct ID3D11DeviceContext;
class IBuffer;
class IDepthStencil;
class IRenderer;
class IRenderTarget;
class IRenderTargetManager;
class IShaderResource;
class IShaderResourceManager;
class ITextureManager;

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
	bool Initialize( IRenderer& renderer );
	void SetRenderTargetDepthStencilView( IRenderer& renderer );
	void ClearDepthStencil( ID3D11DeviceContext* pDeviceContext );
	void ClearRenderTargets( ID3D11DeviceContext* pDeviceContext, const rtClearColor& clearColor );
	void SceneEnd( ID3D11DeviceContext* pDeviceContext );

	CRenderOutputManager( );
private:
	bool CreateDefaultRenderTaraget( IRenderer& renderer );
	bool CreateNormalRenderTarget( IRenderer& renderer );
	bool CreateDepthRenderTarget( IRenderer& renderer );
	bool CreateDefaultDepthStencil( IRenderer& renderer );

	std::array<IShaderResource*, RENDER_OUPUT_TYPE::COUNT>	m_renderSRVs;
	std::array<IRenderTarget*, RENDER_OUPUT_TYPE::COUNT>	m_renderOutputs;
	IDepthStencil*											m_pPrimeDs;
};

