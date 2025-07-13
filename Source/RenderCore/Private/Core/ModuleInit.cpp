#include "AssetFactory.h"
#include "GpuProfiler.h"
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
	Owner<IRenderCore*> GRenderCore = nullptr;
	Owner<UserInterfaceRenderer*> GUiRenderer = nullptr;

	void* GetRenderCore()
	{
		return GRenderCore;
	}

	void* GetUiRenderer()
	{
		return GUiRenderer;
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

	GRenderCore = CreateRenderCore();

	DeferredAssetRegister::GetInstance().Register();

	auto sharedContext = GetInterface<imgui::SharedContext>();
	if ( sharedContext )
	{
		GUiRenderer = CreateUserInterfaceRenderer();
		
		ImGui::SetCurrentContext( sharedContext->m_context );
		ImGui::SetAllocatorFunctions( sharedContext->m_allocFunc, sharedContext->m_freeFunc );
	}
}

RENDERCORE_FUNC_DLL void ShutdownModules()
{
	CleanUpGpuProfiler();

	DestroyUserInterfaceRenderer( GUiRenderer );
	DestroyRenderCore( GRenderCore );

	UnregisterFactory<IGpuProfiler>();
	UnregisterFactory<IRenderCore>();
	UnregisterFactory<UserInterfaceRenderer>();
}
