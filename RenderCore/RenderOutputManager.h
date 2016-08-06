#pragma once

#include <array>
#include <tuple>

class CTextureManager;
class CRenderTargetManager;
struct IDXGISwapChain;
struct ID3D11Device;
struct ID3D11DeviceContext;
class IDepthStencil;
class IRenderer;
class IRenderTarget;

enum RENDER_OUPUT_TYPE
{
	FRAME_BUFFER = 0,
	NORMAL_BUFFER,
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

	CRenderOutputManager( );
	~CRenderOutputManager( );

private:
	bool CreateDefaultRenderTaraget( ID3D11Device* pDevice, IDXGISwapChain* pSwapChain, CTextureManager& textureMgr, CRenderTargetManager& renderTargetMgr );
	bool CreateNormalRenderTarget( ID3D11Device* pDevice, CTextureManager& textureMgr, CRenderTargetManager& renderTargetMgr );
	bool CreateDefaultDepthStencil( ID3D11Device* pDevice, CTextureManager& textureMgr, CRenderTargetManager& renderTargetMgr );

	std::array<IRenderTarget*, RENDER_OUPUT_TYPE::COUNT> m_renderOutputs;
	IDepthStencil* m_pPrimeDs;
};

