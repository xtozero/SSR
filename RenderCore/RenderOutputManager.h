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
	bool Initialize( IRenderer* pRenderer );
	void SetRenderTargetDepthStencilView( IRenderer* pRenderer );
	void ClearDepthStencil( ID3D11DeviceContext* pDeviceContext );
	void ClearRenderTargets( ID3D11DeviceContext* pDeviceContext, const rtClearColor& clearColor );
	void SceneEnd( ID3D11DeviceContext* pDeviceContext );

	CRenderOutputManager( );
private:
	bool CreateDefaultRenderTaraget( ID3D11Device* pDevice, IDXGISwapChain* pSwapChain, ITextureManager& textureMgr, IRenderTargetManager& renderTargetMgr );
	bool CreateNormalRenderTarget( ID3D11Device* pDevice, ITextureManager& textureMgr, IRenderTargetManager& renderTargetMgr, IShaderResourceManager& srMgr );
	bool CreateDepthRenderTarget( ID3D11Device* pDevice, ITextureManager& textureMgr, IRenderTargetManager& renderTargetMgr, IShaderResourceManager& srMgr );
	bool CreateDefaultDepthStencil( ID3D11Device* pDevice, ITextureManager& textureMgr, IRenderTargetManager& renderTargetMgr );

	std::array<IShaderResource*, RENDER_OUPUT_TYPE::COUNT>	m_renderSRVs;
	std::array<IRenderTarget*, RENDER_OUPUT_TYPE::COUNT>	m_renderOutputs;
	IDepthStencil*											m_pPrimeDs;
};

