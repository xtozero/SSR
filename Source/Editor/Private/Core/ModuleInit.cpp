#include "GuideTypes.h"
#include "IEditor.h"
#include "imgui.h"
#include "ImguiSharedContext.h"
#include "InterfaceFactories.h"
#include "LibraryTool/Common.h"

using ::editor::IEditor;

namespace
{
	Owner<IEditor*> GEditor = nullptr;
	imgui::SharedContext GImguiSharedContext;

	void* GetEditor()
	{
		return GEditor;
	}

	void* GetImguiSharedContext()
	{
		return &GImguiSharedContext;
	}
}

namespace editor 
{
	EDITOR_FUNC_DLL void BootUpModules()
	{
		RegisterFactory<IEditor>( &GetEditor );
		RegisterFactory<imgui::SharedContext>( &GetImguiSharedContext );

		GEditor = CreateEditor();
		GImguiSharedContext = imgui::GetSharedContext();

		ImGui::SetCurrentContext( GImguiSharedContext.m_context );
		ImGui::SetAllocatorFunctions( GImguiSharedContext.m_allocFunc, GImguiSharedContext.m_freeFunc );
	}

	EDITOR_FUNC_DLL void ShutdownModules()
	{
		DestroyEditor( GEditor );
		imgui::DestroySharedContext( GImguiSharedContext );

		UnregisterFactory<IEditor>();
		UnregisterFactory<imgui::SharedContext>();
	}
}
