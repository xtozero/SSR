#include "ImguiEditor.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "LibraryTool/InterfaceFactories.h"
#include "Platform/IPlatform.h"

bool ImguiEditor::BootUp( IPlatform& platform )
{
	m_logicDll = LoadModule( "Logic.dll" );
	if ( m_logicDll == nullptr )
	{
		return false;
	}

	m_logic = GetInterface<ILogic>();
	if ( m_logic == nullptr )
	{
		return false;
	}

	if ( m_logic->BootUp( platform ) == false )
	{
		return false;
	}

	return ImGui_ImplWin32_Init( platform.GetRawHandle<void*>() );
}

void ImguiEditor::Update()
{
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Render();

	m_logic->Update();
}

void ImguiEditor::Pause()
{
	m_logic->Pause();
}

void ImguiEditor::Resume()
{
	m_logic->Resume();
}

void ImguiEditor::HandleUserInput( const UserInput& input )
{
	m_logic->HandleUserInput( input );
}

void ImguiEditor::AppSizeChanged( IPlatform& platform )
{
	m_logic->AppSizeChanged( platform );
}

ImguiEditor::~ImguiEditor()
{
	ShutdownModule( m_logicDll );
	ImGui_ImplWin32_Shutdown();
}

Owner<IEditor*> CreateEditor()
{
	return new ImguiEditor();
}

void DestroyEditor( Owner<IEditor*> pEditor )
{
	delete pEditor;
}
