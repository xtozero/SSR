#include "ImguiEditor.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "IPanel.h"
#include "LibraryTool/InterfaceFactories.h"
#include "PanelFactory.h"
#include "Platform/IPlatform.h"
#include "UserInput/UserInput.h"

constexpr int32 ShowImGuiShowCase = 0;

using ::engine::UserInput;
using enum ::engine::UserInputCode;

namespace
{
    ImGuiKey UserInputCodeToImGuiKey( engine::UserInputCode code )
    {
        switch ( code )
        {
        case UIC_TAB: return ImGuiKey_Tab;
        case UIC_LEFT: return ImGuiKey_LeftArrow;
        case UIC_RIGHT: return ImGuiKey_RightArrow;
        case UIC_UP: return ImGuiKey_UpArrow;
        case UIC_DOWN: return ImGuiKey_DownArrow;
        case UIC_PAGEUP: return ImGuiKey_PageUp;
        case UIC_PAGEDOWN: return ImGuiKey_PageDown;
        case UIC_HOME: return ImGuiKey_Home;
        case UIC_END: return ImGuiKey_End;
        case UIC_INSERT: return ImGuiKey_Insert;
        case UIC_DELETE: return ImGuiKey_Delete;
        case UIC_BACKSPACE: return ImGuiKey_Backspace;
        case UIC_SPACEBAR: return ImGuiKey_Space;
        case UIC_ENTER: return ImGuiKey_Enter;
        case UIC_ESCAPE: return ImGuiKey_Escape;
        case UIC_QUOTE: return ImGuiKey_Apostrophe;
        case UIC_COMMA: return ImGuiKey_Comma;
        case UIC_MINUS: return ImGuiKey_Minus;
        case UIC_PERIOD: return ImGuiKey_Period;
        case UIC_SLASH: return ImGuiKey_Slash;
        case UIC_SEMICOLON: return ImGuiKey_Semicolon;
        case UIC_EQUALS: return ImGuiKey_Equal;
        case UIC_LEFTBRACKET: return ImGuiKey_LeftBracket;
        case UIC_BACKSLASH: return ImGuiKey_Backslash;
        case UIC_RIGHTBRACKET: return ImGuiKey_RightBracket;
        case UIC_TILDE: return ImGuiKey_GraveAccent;
        case UIC_CAPSLOCK: return ImGuiKey_CapsLock;
        case UIC_SCROLLLOCK: return ImGuiKey_ScrollLock;
        case UIC_NUMLOCK: return ImGuiKey_NumLock;
        // case VK_SNAPSHOT: return ImGuiKey_PrintScreen;
        case UIC_PAUSE: return ImGuiKey_Pause;
        case UIC_NUM_PAD_ZERO: return ImGuiKey_Keypad0;
        case UIC_NUM_PAD_ONE: return ImGuiKey_Keypad1;
        case UIC_NUM_PAD_TWO: return ImGuiKey_Keypad2;
        case UIC_NUM_PAD_THREE: return ImGuiKey_Keypad3;
        case UIC_NUM_PAD_FOUR: return ImGuiKey_Keypad4;
        case UIC_NUM_PAD_FIVE: return ImGuiKey_Keypad5;
        case UIC_NUM_PAD_SIX: return ImGuiKey_Keypad6;
        case UIC_NUM_PAD_SEVEN: return ImGuiKey_Keypad7;
        case UIC_NUM_PAD_EIGHT: return ImGuiKey_Keypad8;
        case UIC_NUM_PAD_NINE: return ImGuiKey_Keypad9;
        case UIC_DECIMAL: return ImGuiKey_KeypadDecimal;
        case UIC_DIVIDE: return ImGuiKey_KeypadDivide;
        case UIC_MULTIPLY: return ImGuiKey_KeypadMultiply;
        case UIC_SUBTRACT: return ImGuiKey_KeypadSubtract;
        case UIC_ADD: return ImGuiKey_KeypadAdd;
        // case IM_VK_KEYPAD_ENTER: return ImGuiKey_KeypadEnter;
        case UIC_LEFTSHIFT: return ImGuiKey_LeftShift;
        case UIC_LEFTCONTROL: return ImGuiKey_LeftCtrl;
        case UIC_LEFTALT: return ImGuiKey_LeftAlt;
        //case VK_LWIN: return ImGuiKey_LeftSuper;
        case UIC_RIGHTSHIFT: return ImGuiKey_RightShift;
        case UIC_RIGHTCONTROL: return ImGuiKey_RightCtrl;
        case UIC_RIGHTALT: return ImGuiKey_RightAlt;
        //case VK_RWIN: return ImGuiKey_RightSuper;
        //case VK_APPS: return ImGuiKey_Menu;
        case UIC_ZERO: return ImGuiKey_0;
        case UIC_ONE: return ImGuiKey_1;
        case UIC_TWO: return ImGuiKey_2;
        case UIC_THREE: return ImGuiKey_3;
        case UIC_FOUR: return ImGuiKey_4;
        case UIC_FIVE: return ImGuiKey_5;
        case UIC_SIX: return ImGuiKey_6;
        case UIC_SEVEN: return ImGuiKey_7;
        case UIC_EIGHT: return ImGuiKey_8;
        case UIC_NINE: return ImGuiKey_9;
        case UIC_A: return ImGuiKey_A;
        case UIC_B: return ImGuiKey_B;
        case UIC_C: return ImGuiKey_C;
        case UIC_D: return ImGuiKey_D;
        case UIC_E: return ImGuiKey_E;
        case UIC_F: return ImGuiKey_F;
        case UIC_G: return ImGuiKey_G;
        case UIC_H: return ImGuiKey_H;
        case UIC_I: return ImGuiKey_I;
        case UIC_J: return ImGuiKey_J;
        case UIC_K: return ImGuiKey_K;
        case UIC_L: return ImGuiKey_L;
        case UIC_M: return ImGuiKey_M;
        case UIC_N: return ImGuiKey_N;
        case UIC_O: return ImGuiKey_O;
        case UIC_P: return ImGuiKey_P;
        case UIC_Q: return ImGuiKey_Q;
        case UIC_R: return ImGuiKey_R;
        case UIC_S: return ImGuiKey_S;
        case UIC_T: return ImGuiKey_T;
        case UIC_U: return ImGuiKey_U;
        case UIC_V: return ImGuiKey_V;
        case UIC_W: return ImGuiKey_W;
        case UIC_X: return ImGuiKey_X;
        case UIC_Y: return ImGuiKey_Y;
        case UIC_Z: return ImGuiKey_Z;
        case UIC_F1: return ImGuiKey_F1;
        case UIC_F2: return ImGuiKey_F2;
        case UIC_F3: return ImGuiKey_F3;
        case UIC_F4: return ImGuiKey_F4;
        case UIC_F5: return ImGuiKey_F5;
        case UIC_F6: return ImGuiKey_F6;
        case UIC_F7: return ImGuiKey_F7;
        case UIC_F8: return ImGuiKey_F8;
        case UIC_F9: return ImGuiKey_F9;
        case UIC_F10: return ImGuiKey_F10;
        case UIC_F11: return ImGuiKey_F11;
        case UIC_F12: return ImGuiKey_F12;
        default: return ImGuiKey_None;
        }
    }
}

namespace editor
{
    bool ImguiEditor::BootUp( engine::IPlatform& platform )
    {
        m_logicDll = LoadModule( "Logic.dll" );
        if ( m_logicDll == nullptr )
        {
            return false;
        }

        m_logic = GetInterface<logic::ILogic>();
        if ( m_logic == nullptr )
        {
            return false;
        }

        if ( m_logic->BootUp( platform ) == false )
        {
            return false;
        }

        m_panels = std::move( PanelFactory::GetInstance().CreateAllPanel( *this ) );

        return ImGui_ImplWin32_Init( platform.GetRawHandle<void*>() );
    }

    void ImguiEditor::Update()
    {
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::DockSpaceOverViewport( 0, viewport, ImGuiDockNodeFlags_PassthruCentralNode );

        if constexpr ( ShowImGuiShowCase )
        {
            ImGui::ShowDemoWindow();
        }

        for ( auto& panel : m_panels )
        {
            panel->Draw();
        }

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

    void ImguiEditor::HandleUserInput( const engine::UserInput& input )
    {
        if ( ImGui::GetCurrentContext() == nullptr )
        {
            return;
        }

        ImGuiIO& io = ImGui::GetIO();
        switch ( input.m_code )
        {
        case UIC_MOUSE_LEFT:
            [[fallthrough]];
        case UIC_MOUSE_RIGHT:
            [[fallthrough]];
        case UIC_MOUSE_MIDDLE:
        {
            int32 button = 0;

            if ( input.m_code == UIC_MOUSE_LEFT )
            {
                button = 0;
            }
            else if ( input.m_code == UIC_MOUSE_RIGHT )
            {
                button = 1;
            }
            else if ( input.m_code == UIC_MOUSE_MIDDLE )
            {
                button = 2;
            }

            io.AddMouseSourceEvent( ImGuiMouseSource_Mouse );
            io.AddMouseButtonEvent( button, input.m_axis[UserInput::Z_AXIS] < 0 );
            break;
        }
        case UIC_MOUSE_WHEELSPIN:
        {
            io.AddMouseWheelEvent( 0.f, input.m_axis[UserInput::Z_AXIS] );
            break;
        }
        default:
        {
            ImGuiKey imGuiKey = UserInputCodeToImGuiKey( input.m_code );
            if ( imGuiKey != ImGuiKey_None )
            {
                io.AddKeyEvent( imGuiKey, input.m_axis[UserInput::Z_AXIS] < 0 );
            }
            break;
        }
        }

        for ( auto& panel : m_panels )
        {
            panel->HandleUserInput( input );
        }
    }

    void ImguiEditor::HandleTextInput( uint64 text, bool bUnicode )
    {
        ImGuiIO& io = ImGui::GetIO();
        if ( bUnicode )
        {
            io.AddInputCharacterUTF16( static_cast<ImWchar16>( text ) );
        }
        else
        {
            io.AddInputCharacter( static_cast<uint32>( text ) );
        }
    }

    void ImguiEditor::AppSizeChanged( engine::IPlatform& platform )
    {
        m_logic->AppSizeChanged( platform );
    }

    logic::GameClientViewport* ImguiEditor::GetGameClientViewport()
    {
        return m_logic->GetGameClientViewport();
    }

    bool ImguiEditor::LoadWorld( const char* filePath )
    {
        return m_logic->LoadWorld( filePath );
    }

    void ImguiEditor::UnloadWorld()
    {
        m_logic->UnloadWorld();
    }

    logic::World& ImguiEditor::GetWorld()
    {
        return m_logic->GetWorld();
    }

    PanelSharedContext& ImguiEditor::GetPanelSharedCtx()
    {
        return m_ctx;
    }

    ImguiEditor::~ImguiEditor()
    {
        m_panels.clear();

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
}
