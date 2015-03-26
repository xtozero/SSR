#pragma once

#include "common.h"

class RENDERCORE_DLL IRenderer
{
public:
	virtual bool InitializeRenderer ( HWND hWind, UINT nWndWidth, UINT nWndHeight ) = 0;
	virtual void ShutDownRenderer ( ) = 0;
	virtual void ClearRenderTargetView ( ) = 0;
	virtual void ClearRenderTargetView ( float r, float g, float b, float a ) = 0;
	virtual void ClearDepthStencilView ( ) = 0;
	virtual void Present ( ) = 0;

protected:
	IRenderer ( ) {}
public:
	virtual ~IRenderer ( ) {}
};

