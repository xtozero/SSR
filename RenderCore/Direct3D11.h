#pragma once

#include "common.h"
#include "IBuffer.h"
#include "IRenderer.h"
#include "IMesh.h"
#include "MeshLoader.h"
#include "RenderView.h"
#include <D3D11.h>
#include <D3DX11.h>
#include <DXGI.h>
#include <vector>
#include <map>

namespace
{
	RENDERCORE_FUNC_DLL IRenderer* CreateDirect3D11Renderer ( );
};

class BaseMesh;

class CDirect3D11 : public IRenderer
{
public:
	virtual bool InitializeRenderer ( HWND hWind, UINT nWndWidth, UINT nWndHeight );
	virtual void ShutDownRenderer ( );
	virtual void ClearRenderTargetView ( );
	virtual void ClearRenderTargetView ( float r, float g, float b, float a );
	virtual void ClearDepthStencilView ( );
	virtual void SceneBegin( );
	virtual void SceneEnd( );

	virtual IShader* CreateVertexShader( const TCHAR* pFilePath, const char* pProfile );
	virtual IShader* CreatePixelShader( const TCHAR* pFilePath, const char* pProfile );

	virtual IBuffer* CreateVertexBuffer( const UINT stride, const UINT numOfElement, const void* srcData );
	virtual IBuffer* CreateIndexBuffer( const UINT stride, const UINT numOfElement, const void* srcData );
	virtual IBuffer* CreateConstantBuffer( const UINT stride, const UINT numOfElement, const void* srcData );

	virtual IShader* SearchShaderByName( const TCHAR* pName );

	virtual bool InitMaterial( );
	virtual std::shared_ptr<IMesh> GetModelPtr( const TCHAR* pModelName );
	virtual void DrawModel( std::shared_ptr<IMesh> pModel );

	virtual void PushViewPort( const float topLeftX, const float topLeftY, const float width, const float height, const float minDepth = 0.0f, const float maxDepth = 1.0f );
	virtual void PopViewPort( );

	virtual IRenderView* GetCurrentRenderView( );

	virtual void UpdateWorldMatrix( const D3DXMATRIX& worldMatrix );
private:
	bool CreateD3D11Device ( HWND hWind, UINT nWndWidth, UINT nWndHeight );
	bool CreatePrimeRenderTargetVIew ( );
	bool CreatePrimeDepthBuffer ( UINT nWndWidth, UINT nWndHeight );
	bool SetRenderTargetAndDepthBuffer ( );

private:
	ID3D11Device*			m_pd3d11Device;
	ID3D11DeviceContext*	m_pd3d11DeviceContext;
	IDXGISwapChain*			m_pdxgiSwapChain;

	ID3D11RenderTargetView*	m_pd3d11PrimeRTView;

	ID3D11Texture2D*		m_pd3d11PrimeDSBuffer;
	ID3D11DepthStencilView*	m_pd3d11PrimeDSView;

	std::map<String, IShader*> m_shaderList;
	std::vector<IBuffer*>	m_bufferList;

	std::vector<std::shared_ptr<IMesh>>	m_models;

	RenderView				m_view;
	CMeshLoader				m_meshLoader;

	IBuffer*				m_worldMatrixBuffer;
public:
	CDirect3D11 ( );
	virtual ~CDirect3D11 ( );
};

