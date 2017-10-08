#pragma once

#include <array>
#include <tuple>

struct ID3D11DeviceContext;
class IBuffer;
class IRenderer;
class IRenderResource;

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
	void ClearDepthStencil( IRenderer& renderer );
	void ClearRenderTargets( IRenderer& renderer, const rtClearColor& clearColor );
	void SceneEnd( ID3D11DeviceContext* pDeviceContext );

	CRenderOutputManager( );
private:
	bool CreateDefaultRenderTaraget( IRenderer& renderer );
	bool CreateNormalRenderTarget( IRenderer& renderer );
	bool CreateDepthRenderTarget( IRenderer& renderer );
	bool CreateDefaultDepthStencil( IRenderer& renderer );

	std::array<IRenderResource*, RENDER_OUPUT_TYPE::COUNT>	m_renderSRVs;
	std::array<IRenderResource*, RENDER_OUPUT_TYPE::COUNT>	m_renderOutputs;
	IRenderResource*											m_pPrimeDs;
};

