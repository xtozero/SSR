#pragma once

#include "common.h"
#include "IRenderer.h"
#include <D3D11.h>
#include <D3DX11.h>
#include <DXGI.h>

class RENDERCORE_DLL CDirect3D11 : public IRenderer
{
public:
	virtual bool InitializeRenderer ( HWND hWind, UINT nWndWidth, UINT nWndHeight );
	virtual void ShutDownRenderer ( );
	virtual void ClearRenderTargetView ( );
	virtual void ClearRenderTargetView ( float r, float g, float b, float a );
	virtual void ClearDepthStencilView ( );
	virtual void Present ( );
	virtual void Render ();

private:
	bool CreateD3D11Device ( HWND hWind, UINT nWndWidth, UINT nWndHeight );
	bool CreatePrimeRenderTargetVIew ( );
	bool CreatePrimeDepthBuffer ( UINT nWndWidth, UINT nWndHeight );
	bool SetRenderTargetAndDepthBuffer ();

private:
	ID3D11Device*			m_pd3d11Device;
	ID3D11DeviceContext*	m_pd3d11DeviceContext;
	IDXGISwapChain*			m_pdxgiSwapChain;

	ID3D11RenderTargetView*	m_pd3d11PrimeRTView;

	ID3D11Texture2D*		m_pd3d11PrimeDSBuffer;
	ID3D11DepthStencilView* m_pd3d11PrimeDSView;
public:
	CDirect3D11 ();
	virtual ~CDirect3D11 ();
};

