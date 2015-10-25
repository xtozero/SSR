#pragma once

#include "common.h"

class IShader;
class IBuffer;
class IRenderView;

class RENDERCORE_DLL IRenderer
{
public:
	virtual bool InitializeRenderer ( HWND hWind, UINT nWndWidth, UINT nWndHeight ) = 0;
	virtual void ShutDownRenderer ( ) = 0;
	virtual void ClearRenderTargetView ( ) = 0;
	virtual void ClearRenderTargetView ( float r, float g, float b, float a ) = 0;
	virtual void ClearDepthStencilView ( ) = 0;
	virtual void Present ( ) = 0;
	virtual void Render( ) = 0;

	virtual IShader* CreateVertexShader( const TCHAR* pFilePath, const char* pProfile ) = 0;
	virtual IShader* CreatePixelShader( const TCHAR* pFilePath, const char* pProfile ) = 0;

	virtual IBuffer* CreateVertexBuffer( const UINT stride, const UINT numOfElement, const void* srcData ) = 0;
	virtual IBuffer* CreateIndexBuffer( const UINT stride, const UINT numOfElement, const void* srcData ) = 0;

	virtual IShader* SearchShaderByName( const TCHAR* name ) = 0;

	virtual bool InitMaterial( ) = 0;
	virtual bool InitModel( ) = 0;

	virtual void PushViewPort( const float topLeftX, const float topLeftY, const float width, const float height, const float minDepth = 0.0f, const float maxDepth = 1.0f ) = 0;
	virtual void PopViewPort( ) = 0;

	virtual IRenderView* GetCurrentRenderView( ) = 0;
protected:
	IRenderer ( ) {}
public:
	virtual ~IRenderer ( ) {}
};

