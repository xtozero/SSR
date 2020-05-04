#include "stdafx.h"

#include "Core/InterfaceFactories.h"
#include "Renderer/IRenderCore.h"

namespace
{
	Owner<IRenderCore*> g_renderer = nullptr;

	void* GetRenderer( )
	{
		return g_renderer;
	}
}

RENDERCORE_FUNC_DLL void BootUpModules( )
{
	RegisterFactory<IRenderCore>( &GetRenderer );

	g_renderer = CreateRenderCore( );
}

RENDERCORE_FUNC_DLL void ShutdownModules( )
{
	DestoryRenderCore( g_renderer );

	UnregisterFactory<IRenderCore>( );
}