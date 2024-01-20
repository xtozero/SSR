#include "stdafx.h"

#include "AssetFactory.h"
#include "GPUProfiler.h"
#include "ImguiSharedContext.h"
#include "InterfaceFactories.h"
#include "Renderer/IRenderCore.h"
#include "UserInterfaceRenderer.h"

using ::rendercore::GetGpuProfiler;
using ::rendercore::CleanUpGpuProfiler;
using ::rendercore::CreateRenderCore;
using ::rendercore::CreateUserInterfaceRenderer;
using ::rendercore::DestroyRenderCore;
using ::rendercore::DestroyUserInterfaceRenderer;
using ::rendercore::IGpuProfiler;
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

	void* GetGpuProfilerPtr()
	{
		return &GetGpuProfiler();
	}
}

RENDERCORE_FUNC_DLL void BootUpModules()
{
	RegisterFactory<IGpuProfiler>( &GetGpuProfilerPtr );
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
	CleanUpGpuProfiler();

	DestroyUserInterfaceRenderer( g_uiRenderer );
	DestroyRenderCore( g_renderCore );

	UnregisterFactory<IGpuProfiler>();
	UnregisterFactory<IRenderCore>();
	UnregisterFactory<UserInterfaceRenderer>();
}
