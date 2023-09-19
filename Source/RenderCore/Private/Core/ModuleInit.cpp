#include "stdafx.h"

#include "AssetFactory.h"
#include "ImguiSharedContext.h"
#include "InterfaceFactories.h"
#include "Renderer/IRenderCore.h"
#include "UserInterfaceRenderer.h"

using ::rendercore::CreateRenderCore;
using ::rendercore::CreateUserInterfaceRenderer;
using ::rendercore::DestoryRenderCore;
using ::rendercore::DestoryUserInterfaceRenderer;
using ::rendercore::IRenderCore;
using ::rendercore::UserInterfaceRenderer;

namespace
{
	Owner<IRenderCore*> g_renderCore = nullptr;
	Owner<UserInterfaceRenderer*> g_uiRenderer = nullptr;

	void* GetRenderCore()
	{
		return g_renderCore;
	}

	void* GetUiRenderer()
	{
		return g_uiRenderer;
	}
}

RENDERCORE_FUNC_DLL void BootUpModules()
{
	RegisterFactory<IRenderCore>( &GetRenderCore );
	RegisterFactory<UserInterfaceRenderer>( &GetUiRenderer );

	g_renderCore = CreateRenderCore();

	DeferredAssetRegister::GetInstance().Register();

	auto sharedContext = GetInterface<imgui::SharedContext>();
	if ( sharedContext )
	{
		g_uiRenderer = CreateUserInterfaceRenderer();
		
		ImGui::SetCurrentContext( sharedContext->m_context );
		ImGui::SetAllocatorFunctions( sharedContext->m_allocFunc, sharedContext->m_freeFunc );
	}
}

RENDERCORE_FUNC_DLL void ShutdownModules()
{
	DestoryUserInterfaceRenderer( g_uiRenderer );
	DestoryRenderCore( g_renderCore );

	UnregisterFactory<UserInterfaceRenderer>();
	UnregisterFactory<IRenderCore>();
}
