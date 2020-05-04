#pragma once

#include "Util.h"

class IRenderCore
{
public:
	virtual bool BootUp( HWND hWnd, UINT nWndWidth, UINT nWndHeight ) = 0;
	virtual void HandleDeviceLost( HWND hWnd, UINT nWndWidth, UINT nWndHeight ) = 0;
	virtual void AppSizeChanged( UINT nWndWidth, UINT nWndHeight ) = 0;

	virtual ~IRenderCore( ) = default;

protected:
	IRenderCore( ) = default;
};

Owner<IRenderCore*> CreateRenderCore( );
void DestoryRenderCore( Owner<IRenderCore*> pRenderCore );
