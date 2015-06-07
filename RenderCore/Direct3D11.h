#pragma once

#include "common.h"
#include "IBuffer.h"
#include "IRenderer.h"
#include <D3D11.h>
#include <D3DX11.h>
#include <DXGI.h>
#include <vector>
#include <map>

namespace
{
	RENDERCORE_FUNC_DLL IRenderer* CreateDirect3D11Renderer ( );
};

class RENDERCORE_DLL CDirect3D11 : public IRenderer
{
public:
	virtual bool InitializeRenderer ( HWND hWind, UINT nWndWidth, UINT nWndHeight );
	virtual void ShutDownRenderer ( );
	virtual void ClearRenderTargetView ( );
	virtual void ClearRenderTargetView ( float r, float g, float b, float a );
	virtual void ClearDepthStencilView ( );
	virtual void Present ( );
	virtual void Render ( );

	virtual IShader* CreateVertexShader( const TCHAR* pFilePath, const char* pProfile );
	virtual IShader* CreatePixelShader( const TCHAR* pFilePath, const char* pProfile );

	virtual IBuffer* CreateVertexBuffer( const UINT stride, const UINT numOfElement, const void* srcData );
	virtual IBuffer* CreateIndexBuffer( const UINT stride, const UINT numOfElement, const void* srcData );

	virtual IShader* SearchShaderByName( const TCHAR* name );

	virtual void PushViewPort( const float topLeftX, const float topLeftY, const float width, const float height, const float minDepth = 0.0f, const float maxDepth = 1.0f );
	virtual void PopViewPort( );
private:
	bool CreateD3D11Device ( HWND hWind, UINT nWndWidth, UINT nWndHeight );
	bool CreatePrimeRenderTargetVIew ( );
	bool CreatePrimeDepthBuffer ( UINT nWndWidth, UINT nWndHeight );
	bool SetRenderTargetAndDepthBuffer ( );

	void SetViewPort( );

private:
	ID3D11Device*			m_pd3d11Device;
	ID3D11DeviceContext*	m_pd3d11DeviceContext;
	IDXGISwapChain*			m_pdxgiSwapChain;

	ID3D11RenderTargetView*	m_pd3d11PrimeRTView;

	ID3D11Texture2D*		m_pd3d11PrimeDSBuffer;
	ID3D11DepthStencilView* m_pd3d11PrimeDSView;

	std::map<String, IShader*> m_shaderList;
	std::vector<IBuffer*> m_bufferList;

	std::vector<D3D11_VIEWPORT> m_viewportList;
public:
	CDirect3D11 ( );
	virtual ~CDirect3D11 ( );
};

