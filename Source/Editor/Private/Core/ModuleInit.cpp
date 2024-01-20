#include "common.h"
#include "GuideTypes.h"
#include "IEditor.h"
#include "imgui.h"
#include "ImguiSharedContext.h"
#include "InterfaceFactories.h"

using ::editor::IEditor;

namespace
{
	Owner<IEditor*> g_editor = nullptr;
	imgui::SharedContext g_imguiSharedContext;

	void* GetEditor()
	{
		return g_editor;
	}

	void* GetImguiSharedContext()
	{
		return &g_imguiSharedContext;
	}
}

namespace editor 
{
	EDITOR_FUNC_DLL void BootUpModules()
	{
		RegisterFactory<IEditor>( &GetEditor );
		RegisterFactory<imgui::SharedContext>( &GetImguiSharedContext );

		g_editor = CreateEditor();
		g_imguiSharedContext = imgui::GetSharedContext();

		ImGui::SetCurrentContext( g_imguiSharedContext.m_context );
		ImGui::SetAllocatorFunctions( g_imguiSharedContext.m_allocFunc, g_imguiSharedContext.m_freeFunc );
	}

	EDITOR_FUNC_DLL void ShutdownModules()
	{
		DestroyEditor( g_editor );
		imgui::DestroySharedContext( g_imguiSharedContext );

		UnregisterFactory<IEditor>();
		UnregisterFactory<imgui::SharedContext>();
	}
}
