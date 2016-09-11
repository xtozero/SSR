#pragma once

#include "IBuffer.h"
#include "IRenderer.h"
#include "IMaterial.h"
#include "IMesh.h"
#include "MeshBuilder.h"
#include "MeshLoader.h"
#include "RenderEffect.h"
#include "RenderOutputManager.h"
#include "RenderTargetManager.h"
#include "RenderView.h"
#include "ShaderListScriptLoader.h"
#include "ShaderResourceManager.h"
#include "SnapShotManager.h"
#include "SurfaceManager.h"
#include "TextureManager.h"

#include <vector>
#include <map>
#include <mutex>

class BaseMesh;
class IDepthStencilStateFactory;
class IRasterizerStateFactory;
class ISamplerStateFactory;
class CMaterialLoader;

class CDirect3D11 : public IRenderer
{
public:
	virtual bool InitializeRenderer( HWND hWind, UINT nWndWidth, UINT nWndHeight ) override;
	virtual void ShutDownRenderer( ) override;
	virtual void SceneBegin( ) override;
	virtual void SceneEnd( ) override;

	virtual std::shared_ptr<IShader> CreateVertexShader( const TCHAR* pFilePath, const char* pProfile ) override;
	virtual std::shared_ptr<IShader> CreatePixelShader( const TCHAR* pFilePath, const char* pProfile ) override;

	virtual std::shared_ptr<IBuffer> CreateVertexBuffer( const UINT stride, const UINT numOfElement, const void* srcData ) override;
	virtual std::shared_ptr<IBuffer> CreateIndexBuffer( const UINT stride, const UINT numOfElement, const void* srcData ) override;
	virtual std::shared_ptr<IBuffer> CreateConstantBuffer( const String& bufferName, const UINT stride, const UINT numOfElement, const void* srcData ) override;

	virtual void* MapConstantBuffer( const String& bufferName ) override;
	virtual void UnMapConstantBuffer( const String& bufferName ) override;
	virtual void SetConstantBuffer( const String& bufferName, const UINT slot, const SHADER_TYPE type ) override;

	virtual std::shared_ptr<IShader> SearchShaderByName( const TCHAR* pName ) override;

	virtual IMaterial* GetMaterialPtr( const TCHAR* pMaterialName ) override;
	virtual std::shared_ptr<IMesh> GetModelPtr( const TCHAR* pModelName ) override;
	virtual void SetModelPtr( const String& modelName, const std::shared_ptr<IMesh>& pModel ) override;
	virtual void DrawModel( std::shared_ptr<IMesh> pModel ) override;

	virtual void PushViewPort( const float topLeftX, const float topLeftY, const float width, const float height, const float minDepth = 0.0f, const float maxDepth = 1.0f ) override;
	virtual void PopViewPort( ) override;

	virtual IRenderView* GetCurrentRenderView( ) override;

	virtual void UpdateWorldMatrix( const D3DXMATRIX& worldMatrix, const D3DXMATRIX& invWorldMatrix ) override;
	virtual std::shared_ptr<IRenderState> CreateRenderState( const String& stateName ) override;

	virtual IShaderResource* GetShaderResourceFromFile( const String& fileName ) override;
	virtual std::shared_ptr<IRenderState> CreateSamplerState( const String& stateName ) override;

	virtual std::shared_ptr<IRenderState> CreateDepthStencilState( const String& stateName ) override;

	virtual void ResetResource( const std::shared_ptr<IMesh>& pMesh, const SHADER_TYPE type ) override;

	virtual void TakeSnapshot2D( const String& sourceTextureName, const String& destTextureName ) override;

	virtual IRendererShadowManager* GetShadowManager( ) override { return m_pShadowManager.get( ); }

	virtual ID3D11Device* GetDevice( ) const override { return m_pd3d11Device.Get( ); }
	virtual IDXGISwapChain* GetSwapChain( ) const override { return m_pdxgiSwapChain.Get( ); }
	virtual ID3D11DeviceContext* GetDeviceContext( ) const override { return m_pd3d11DeviceContext.Get( ); };
	virtual IRenderTargetManager& GetRenderTargetManager( ) override { return m_renderTargetManager; }
	virtual ITextureManager& GetTextureManager( ) override { return m_textureManager; }
	virtual IShaderResourceManager& GetShaderResourceManager( ) override { return m_shaderResourceManager; }
	virtual IMeshBuilder& GetMeshBuilder( ) override;
	virtual ISnapshotManager& GetSnapshotManager( ) override { return m_snapshotManager; }
private:
	bool CreateD3D11Device ( HWND hWind, UINT nWndWidth, UINT nWndHeight );
	bool CreatePrimeRenderTargetVIew ( );
	bool CreatePrimeDepthBuffer ( UINT nWndWidth, UINT nWndHeight );
	void ReportLiveDevice( );
	bool InitializeShaders( );
	bool InitializeMaterial( );
	void RegisterEnumString( );

	Microsoft::WRL::ComPtr<ID3D11Device>			m_pd3d11Device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>		m_pd3d11DeviceContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain>			m_pdxgiSwapChain;

	std::map<String, std::shared_ptr<IShader>>		m_shaderList;
	std::vector<std::shared_ptr<IBuffer>>			m_bufferList;
	std::map<String, std::shared_ptr<IBuffer>>		m_constantBufferList;

	std::unique_ptr<RenderView>						m_pView;
	CMeshLoader										m_meshLoader;

	std::shared_ptr<IBuffer>						m_pWorldMatrixBuffer;

	CTextureManager									m_textureManager;
	CShaderResourceManager							m_shaderResourceManager;
	CRenderTargetManager							m_renderTargetManager;
	CSnapshotManager								m_snapshotManager;
	CRenderOutputManager							m_renderOutput;

	CShaderListScriptLoader							m_shaderLoader;

	std::unique_ptr<IDepthStencilStateFactory>		m_pDepthStencilFactory;
	std::unique_ptr<IRasterizerStateFactory>		m_pRasterizerFactory;
	std::unique_ptr<ISamplerStateFactory>			m_pSamplerFactory;
	std::unique_ptr<CMaterialLoader>				m_pMaterialLoader;

	CSurfaceManager									m_surfaceManager;

	CRenderEffect									m_renderEffect;
	std::unique_ptr<IRendererShadowManager>			m_pShadowManager;

	CMeshBuilder									m_meshBuilder;
public:
	CDirect3D11 ( );
	virtual ~CDirect3D11 ( );
};

