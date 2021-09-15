#pragma once

#include "Util.h"

class IScene;
class RenderViewGroup;

class IRenderCore
{
public:
	virtual bool BootUp( ) = 0;
	virtual void HandleDeviceLost( ) = 0;
	virtual void AppSizeChanged( ) = 0;

	virtual IScene* CreateScene( ) = 0;
	virtual void RemoveScene( IScene* scene ) = 0;

	virtual void BeginRenderingViewGroup( RenderViewGroup& renderViewGroup ) = 0;

	virtual ~IRenderCore( ) = default;

protected:
	IRenderCore( ) = default;
};

Owner<IRenderCore*> CreateRenderCore( );
void DestoryRenderCore( Owner<IRenderCore*> pRenderCore );
