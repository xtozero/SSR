#include "stdafx.h"

#include "AssetFactory.h"
#include "InterfaceFactories.h"
#include "Renderer/IRenderCore.h"

using rendercore::CreateRenderCore;
using rendercore::DestoryRenderCore;
using rendercore::IRenderCore;

namespace
{
	Owner<IRenderCore*> g_renderer = nullptr;

	void* GetRenderer()
	{
		return g_renderer;
	}
}

RENDERCORE_FUNC_DLL void BootUpModules()
{
	RegisterFactory<IRenderCore>( &GetRenderer );
	g_renderer = CreateRenderCore();

	DeferredAssetRegister::GetInstance().Register();
}

RENDERCORE_FUNC_DLL void ShutdownModules()
{
	DestoryRenderCore( g_renderer );
	UnregisterFactory<IRenderCore>();
}
