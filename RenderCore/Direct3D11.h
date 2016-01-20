#pragma once

#include "common.h"
#include "IBuffer.h"
#include "IRenderer.h"
#include "IMaterial.h"
#include "IMesh.h"
#include "MeshLoader.h"
#include "RenderView.h"
#include "TextureManager.h"
#include <wrl/client.h>
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
	virtual bool InitializeRenderer ( HWND hWind, UINT nWndWidth, UINT nWndHeight ) override;
	virtual void ShutDownRenderer( ) override;
	virtual void ClearRenderTargetView( ) override;
	virtual void ClearRenderTargetView( float r, float g, float b, float a ) override;
	virtual void ClearDepthStencilView( ) override;
	virtual void SceneBegin( ) override;
	virtual void SceneEnd( ) override;

	virtual IShader* CreateVertexShader( const TCHAR* pFilePath, const char* pProfile ) override;
	virtual IShader* CreatePixelShader( const TCHAR* pFilePath, const char* pProfile ) override;

	virtual IBuffer* CreateVertexBuffer( const UINT stride, const UINT numOfElement, const void* srcData ) override;
	virtual IBuffer* CreateIndexBuffer( const UINT stride, const UINT numOfElement, const void* srcData ) override;
	virtual IBuffer* CreateConstantBuffer( const UINT stride, const UINT numOfElement, const void* srcData ) override;

	virtual IShader* SearchShaderByName( const TCHAR* pName ) override;

	virtual bool InitMaterial( ) override;
	virtual std::shared_ptr<IMaterial> GetMaterialPtr( const TCHAR* pMaterialName ) override;
	virtual std::shared_ptr<IMesh> GetModelPtr( const TCHAR* pModelName ) override;
	virtual void DrawModel( std::shared_ptr<IMesh> pModel ) override;

	virtual void PushViewPort( const float topLeftX, const float topLeftY, const float width, const float height, const float minDepth = 0.0f, const float maxDepth = 1.0f ) override;
	virtual void PopViewPort( ) override;

	virtual IRenderView* GetCurrentRenderView( ) override;

	virtual void UpdateWorldMatrix( const D3DXMATRIX& worldMatrix ) override;
	virtual ID3D11RasterizerState* CreateRenderState( bool isWireFrame, bool isAntialiasedLine ) override;

	virtual std::shared_ptr<ITexture> GetTextureFromFile( const String& fileName ) override;
	virtual ID3D11SamplerState* CreateSampler( ) override;
private:
	bool CreateD3D11Device ( HWND hWind, UINT nWndWidth, UINT nWndHeight );
	bool CreatePrimeRenderTargetVIew ( );
	bool CreatePrimeDepthBuffer ( UINT nWndWidth, UINT nWndHeight );
	bool SetRenderTargetAndDepthBuffer ( );
	void ReportLiveDevice( );

private:
	Microsoft::WRL::ComPtr<ID3D11Device>			m_pd3d11Device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>		m_pd3d11DeviceContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain>			m_pdxgiSwapChain;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	m_pd3d11PrimeRTView;

	Microsoft::WRL::ComPtr<ID3D11Texture2D>			m_pd3d11PrimeDSBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	m_pd3d11PrimeDSView;

	std::map<String, IShader*> m_shaderList;
	std::vector<IBuffer*>	m_bufferList;

	std::unique_ptr<RenderView>	m_pView;
	CMeshLoader				m_meshLoader;

	IBuffer*				m_worldMatrixBuffer;

	CTextureManager			m_textureManager;
public:
	CDirect3D11 ( );
	virtual ~CDirect3D11 ( );
};

