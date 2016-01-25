#pragma once

#include "common.h"
#include <d3dx9math.h>
#include <memory>

class IShader;
class IBuffer;
class IRenderView;
class IMaterial;
class IMesh;
class ITexture;
struct ID3D11RasterizerState;

class RENDERCORE_DLL IRenderer
{
public:
	virtual bool InitializeRenderer ( HWND hWind, UINT nWndWidth, UINT nWndHeight ) = 0;
	virtual void ShutDownRenderer ( ) = 0;
	virtual void ClearRenderTargetView ( ) = 0;
	virtual void ClearRenderTargetView ( float r, float g, float b, float a ) = 0;
	virtual void ClearDepthStencilView ( ) = 0;
	virtual void SceneBegin( ) = 0;
	virtual void SceneEnd( ) = 0;

	virtual std::shared_ptr<IShader> CreateVertexShader( const TCHAR* pFilePath, const char* pProfile ) = 0;
	virtual std::shared_ptr<IShader> CreatePixelShader( const TCHAR* pFilePath, const char* pProfile ) = 0;

	virtual std::shared_ptr<IBuffer> CreateVertexBuffer( const UINT stride, const UINT numOfElement, const void* srcData ) = 0;
	virtual std::shared_ptr<IBuffer> CreateIndexBuffer( const UINT stride, const UINT numOfElement, const void* srcData ) = 0;
	virtual std::shared_ptr<IBuffer> CreateConstantBuffer( const UINT stride, const UINT numOfElement, const void* srcData ) = 0;

	virtual std::shared_ptr<IShader> SearchShaderByName( const TCHAR* name ) = 0;

	virtual std::shared_ptr<IMaterial> GetMaterialPtr( const TCHAR* pMaterialName ) = 0;
	virtual std::shared_ptr<IMesh> GetModelPtr( const TCHAR* pModelName ) = 0;
	virtual void DrawModel( std::shared_ptr<IMesh> pModel ) = 0;

	virtual void PushViewPort( const float topLeftX, const float topLeftY, const float width, const float height, const float minDepth = 0.0f, const float maxDepth = 1.0f ) = 0;
	virtual void PopViewPort( ) = 0;

	virtual IRenderView* GetCurrentRenderView( ) = 0;

	virtual void UpdateWorldMatrix( const D3DXMATRIX& worldMatrix ) = 0;

	virtual ID3D11RasterizerState* CreateRenderState( bool isWireFrame, bool isAntialiasedLine ) = 0;
	virtual std::shared_ptr<ITexture> GetTextureFromFile( const String& fileName ) = 0;

	virtual ID3D11SamplerState* CreateSampler( ) = 0;
protected:
	IRenderer ( ) {}
public:
	virtual ~IRenderer ( ) {}
};

