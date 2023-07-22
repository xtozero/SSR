#include "ImguiEditor.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "LibraryTool/InterfaceFactories.h"
#include "Platform/IPlatform.h"
#include "UserInput/UserInput.h"

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
	if ( ImGui::IsWindowHovered( ImGuiHoveredFlags_AnyWindow | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem | ImGuiHoveredFlags_AllowWhenBlockedByPopup ) )
	{
		if ( ImGui::GetCurrentContext() == nullptr )
		{
			return;
		}

		ImGuiIO& io = ImGui::GetIO();
		switch ( input.m_code )
		{
		case UserInputCode::UIC_MOUSE_LEFT:
		case UserInputCode::UIC_MOUSE_RIGHT:
		case UserInputCode::UIC_MOUSE_MIDDLE:
		{
			int32 button = 0;

			if ( input.m_code == UserInputCode::UIC_MOUSE_LEFT )
			{
				button = 0;
			}
			else if ( input.m_code == UserInputCode::UIC_MOUSE_RIGHT )
			{
				button = 1;
			}
			else if ( input.m_code == UserInputCode::UIC_MOUSE_MIDDLE )
			{
				button = 2;
			}

			io.AddMouseSourceEvent( ImGuiMouseSource_Mouse );
			io.AddMouseButtonEvent( button, input.m_axis[UserInput::Z_AXIS] < 0 );
			break;
		}
		case UserInputCode::UIC_MOUSE_WHEELSPIN:
		{
			io.AddMouseWheelEvent( 0.f, input.m_axis[UserInput::Z_AXIS] );
		}
		default:
			break;
		}
	}
	else
	{
		m_logic->HandleUserInput( input );
	}
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
