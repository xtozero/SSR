#pragma once

#include "IBuffer.h"
#include "IRenderer.h"
#include "IMaterial.h"
#include "IMesh.h"
#include "IMeshBuilder.h"
#include "MeshLoader.h"
#include "RenderView.h"
#include "ShaderListScriptLoader.h"
#include "TextureManager.h"

#include <vector>
#include <map>

namespace
{
	RENDERCORE_FUNC_DLL IRenderer* CreateDirect3D11Renderer ( );
	RENDERCORE_FUNC_DLL IMeshBuilder* GetMeshBuilder( );
};

class BaseMesh;
class IDepthStencilStateFactory;
class IRasterizerStateFactory;
class ISamplerStateFactory;

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

	virtual std::shared_ptr<IShader> CreateVertexShader( const TCHAR* pFilePath, const char* pProfile ) override;
	virtual std::shared_ptr<IShader> CreatePixelShader( const TCHAR* pFilePath, const char* pProfile ) override;

	virtual std::shared_ptr<IBuffer> CreateVertexBuffer( const UINT stride, const UINT numOfElement, const void* srcData ) override;
	virtual std::shared_ptr<IBuffer> CreateIndexBuffer( const UINT stride, const UINT numOfElement, const void* srcData ) override;
	virtual std::shared_ptr<IBuffer> CreateConstantBuffer( const UINT stride, const UINT numOfElement, const void* srcData ) override;

	virtual std::shared_ptr<IShader> SearchShaderByName( const TCHAR* pName ) override;

	virtual std::shared_ptr<IMaterial> GetMaterialPtr( const TCHAR* pMaterialName ) override;
	virtual std::shared_ptr<IMesh> GetModelPtr( const TCHAR* pModelName ) override;
	virtual void DrawModel( std::shared_ptr<IMesh> pModel ) override;

	virtual void PushViewPort( const float topLeftX, const float topLeftY, const float width, const float height, const float minDepth = 0.0f, const float maxDepth = 1.0f ) override;
	virtual void PopViewPort( ) override;

	virtual IRenderView* GetCurrentRenderView( ) override;

	virtual void UpdateWorldMatrix( const D3DXMATRIX& worldMatrix ) override;
	virtual Microsoft::WRL::ComPtr<ID3D11RasterizerState> CreateRenderState( const String& stateName ) override;

	virtual std::shared_ptr<ITexture> GetTextureFromFile( const String& fileName ) override;
	virtual Microsoft::WRL::ComPtr<ID3D11SamplerState> CreateSamplerState( const String& stateName ) override;

	virtual  Microsoft::WRL::ComPtr<ID3D11DepthStencilState> CreateDepthStencilState( const String& stateName ) override;
private:
	bool CreateD3D11Device ( HWND hWind, UINT nWndWidth, UINT nWndHeight );
	bool CreatePrimeRenderTargetVIew ( );
	bool CreatePrimeDepthBuffer ( UINT nWndWidth, UINT nWndHeight );
	bool SetRenderTargetAndDepthBuffer ( );
	void ReportLiveDevice( );
	bool InitializeShaders( );
	bool InitializeMaterial( );

private:
	Microsoft::WRL::ComPtr<ID3D11Device>			m_pd3d11Device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>		m_pd3d11DeviceContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain>			m_pdxgiSwapChain;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	m_pd3d11PrimeRTView;

	Microsoft::WRL::ComPtr<ID3D11Texture2D>			m_pd3d11PrimeDSBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	m_pd3d11PrimeDSView;

	std::map<String, std::shared_ptr<IShader>>		m_shaderList;
	std::vector<std::shared_ptr<IBuffer>>			m_bufferList;

	std::unique_ptr<RenderView>						m_pView;
	CMeshLoader										m_meshLoader;

	std::shared_ptr<IBuffer>						m_pWorldMatrixBuffer;

	CTextureManager									m_textureManager;
	CShaderListScriptLoader							m_shaderLoader;

	std::unique_ptr<IDepthStencilStateFactory>		m_pDepthStencilFactory;
	std::unique_ptr<IRasterizerStateFactory>		m_pRasterizerFactory;
	std::unique_ptr<ISamplerStateFactory>			m_pSamplerFactory;
public:
	CDirect3D11 ( );
	virtual ~CDirect3D11 ( );
};

