#include "stdafx.h"

#include "Core/InterfaceFactories.h"
#include "GraphicsResource/RenderOptionManager.h"
#include "Renderer/IRenderCore.h"

namespace
{
	Owner<IRenderCore*> g_renderer = nullptr;

	void* GetRenderer( )
	{
		return g_renderer;
	}

	Owner<IRenderOptionManager*> g_renderOptionManager = nullptr;

	void* GetRenderOptionManager( )
	{
		return g_renderOptionManager;
	}
}

RENDERCORE_FUNC_DLL void BootUpModules( )
{
	RegisterFactory<IRenderCore>( &GetRenderer );
	RegisterFactory<IRenderOptionManager>( &GetRenderOptionManager );

	g_renderer = CreateRenderCore( );
	g_renderOptionManager = CreateRenderOptionManager( );
}

RENDERCORE_FUNC_DLL void ShutdownModules( )
{
	DestoryRenderOptionManager( g_renderOptionManager );
	DestoryRenderCore( g_renderer );

	UnregisterFactory<IRenderOptionManager>( );
	UnregisterFactory<IRenderCore>( );
}