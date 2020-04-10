#include "stdafx.h"

#include "Core/InterfaceFactories.h"
#include "Render/IRenderer.h"

namespace
{
	IRenderer* g_renderer = nullptr;

	void* GetRenderer( )
	{
		return g_renderer;
	}
}

RENDERCORE_FUNC_DLL void BootUpModules( )
{
	RegisterFactory<IRenderer>( &GetRenderer );

	g_renderer = CreateRenderer( );
}

RENDERCORE_FUNC_DLL void ShutdownModules( )
{
	DestoryRenderer( g_renderer );

	UnregisterFactory<IRenderer>( );
}