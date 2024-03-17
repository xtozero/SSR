#include "WindowPlatformInputConvertor.h"

#include "Core/ILogic.h"

#include <windowsx.h>

namespace
{
	void RegisterWindowInputEnumString()
	{
		// mouse input
		REGISTER_ENUM_STRING( WM_MOUSEMOVE );
		REGISTER_ENUM_STRING( WM_LBUTTONDOWN );
		REGISTER_ENUM_STRING( WM_LBUTTONUP );
		REGISTER_ENUM_STRING( WM_LBUTTONDBLCLK );
		REGISTER_ENUM_STRING( WM_RBUTTONDOWN );
		REGISTER_ENUM_STRING( WM_RBUTTONUP );
		REGISTER_ENUM_STRING( WM_RBUTTONDBLCLK );
		REGISTER_ENUM_STRING( WM_MBUTTONDOWN );
		REGISTER_ENUM_STRING( WM_MBUTTONUP );
		REGISTER_ENUM_STRING( WM_MBUTTONDBLCLK );
		REGISTER_ENUM_STRING( WM_MOUSEWHEEL );
		REGISTER_ENUM_STRING( WM_XBUTTONDOWN );
		REGISTER_ENUM_STRING( WM_XBUTTONUP );
		REGISTER_ENUM_STRING( WM_XBUTTONDBLCLK );
		
		// keyboard input
		REGISTER_ENUM_STRING( WM_KEYDOWN );
		REGISTER_ENUM_STRING( WM_KEYUP );

		// virtual keys		
		REGISTER_ENUM_STRING( VK_LBUTTON );
		REGISTER_ENUM_STRING( VK_RBUTTON );
		REGISTER_ENUM_STRING( VK_CANCEL );
		REGISTER_ENUM_STRING( VK_MBUTTON );
		REGISTER_ENUM_STRING( VK_XBUTTON1 );
		REGISTER_ENUM_STRING( VK_XBUTTON2 );
		REGISTER_ENUM_STRING( VK_BACK );
		REGISTER_ENUM_STRING( VK_TAB );
		REGISTER_ENUM_STRING( VK_CLEAR );
		REGISTER_ENUM_STRING( VK_RETURN );

		REGISTER_ENUM_STRING( VK_SHIFT );
		REGISTER_ENUM_STRING( VK_CONTROL );
		REGISTER_ENUM_STRING( VK_MENU );
		REGISTER_ENUM_STRING( VK_PAUSE );
		REGISTER_ENUM_STRING( VK_CAPITAL );

		REGISTER_ENUM_STRING( VK_ESCAPE );

		REGISTER_ENUM_STRING( VK_CONVERT );
		REGISTER_ENUM_STRING( VK_NONCONVERT );
		REGISTER_ENUM_STRING( VK_ACCEPT );
		REGISTER_ENUM_STRING( VK_MODECHANGE );

		REGISTER_ENUM_STRING( VK_SPACE );
		REGISTER_ENUM_STRING( VK_PRIOR );
		REGISTER_ENUM_STRING( VK_NEXT );
		REGISTER_ENUM_STRING( VK_END );
		REGISTER_ENUM_STRING( VK_HOME );
		REGISTER_ENUM_STRING( VK_LEFT );
		REGISTER_ENUM_STRING( VK_UP );
		REGISTER_ENUM_STRING( VK_RIGHT );
		REGISTER_ENUM_STRING( VK_DOWN );
		REGISTER_ENUM_STRING( VK_SELECT );
		REGISTER_ENUM_STRING( VK_PRINT );
		REGISTER_ENUM_STRING( VK_EXECUTE );
		REGISTER_ENUM_STRING( VK_SNAPSHOT );
		REGISTER_ENUM_STRING( VK_INSERT );
		REGISTER_ENUM_STRING( VK_DELETE );
		REGISTER_ENUM_STRING( VK_HELP );

		enum
		{
			VK_0 = '0',
			VK_1,
			VK_2,
			VK_3,
			VK_4,
			VK_5,
			VK_6,
			VK_7,
			VK_8,
			VK_9,
		};

		REGISTER_ENUM_STRING( VK_0 );
		REGISTER_ENUM_STRING( VK_1 );
		REGISTER_ENUM_STRING( VK_2 );
		REGISTER_ENUM_STRING( VK_3 );
		REGISTER_ENUM_STRING( VK_4 );
		REGISTER_ENUM_STRING( VK_5 );
		REGISTER_ENUM_STRING( VK_6 );
		REGISTER_ENUM_STRING( VK_7 );
		REGISTER_ENUM_STRING( VK_8 );
		REGISTER_ENUM_STRING( VK_9 );

		enum
		{
			VK_A = 'A',
			VK_B,
			VK_C,
			VK_D,
			VK_E,
			VK_F,
			VK_G,
			VK_H,
			VK_I,
			VK_J,
			VK_K,
			VK_L,
			VK_M,
			VK_N,
			VK_O,
			VK_P,
			VK_Q,
			VK_R,
			VK_S,
			VK_T,
			VK_U,
			VK_V,
			VK_W,
			VK_X,
			VK_Y,
			VK_Z,
		};

		REGISTER_ENUM_STRING( VK_A );
		REGISTER_ENUM_STRING( VK_B );
		REGISTER_ENUM_STRING( VK_C );
		REGISTER_ENUM_STRING( VK_D );
		REGISTER_ENUM_STRING( VK_E );
		REGISTER_ENUM_STRING( VK_F );
		REGISTER_ENUM_STRING( VK_G );
		REGISTER_ENUM_STRING( VK_H );
		REGISTER_ENUM_STRING( VK_I );
		REGISTER_ENUM_STRING( VK_J );
		REGISTER_ENUM_STRING( VK_K );
		REGISTER_ENUM_STRING( VK_L );
		REGISTER_ENUM_STRING( VK_M );
		REGISTER_ENUM_STRING( VK_N );
		REGISTER_ENUM_STRING( VK_O );
		REGISTER_ENUM_STRING( VK_P );
		REGISTER_ENUM_STRING( VK_Q );
		REGISTER_ENUM_STRING( VK_R );
		REGISTER_ENUM_STRING( VK_S );
		REGISTER_ENUM_STRING( VK_T );
		REGISTER_ENUM_STRING( VK_U );
		REGISTER_ENUM_STRING( VK_V );
		REGISTER_ENUM_STRING( VK_W );
		REGISTER_ENUM_STRING( VK_X );
		REGISTER_ENUM_STRING( VK_Y );
		REGISTER_ENUM_STRING( VK_Z );

		REGISTER_ENUM_STRING( VK_LWIN );
		REGISTER_ENUM_STRING( VK_RWIN );
		REGISTER_ENUM_STRING( VK_APPS );
		REGISTER_ENUM_STRING( VK_SLEEP );
		REGISTER_ENUM_STRING( VK_NUMPAD0 );
		REGISTER_ENUM_STRING( VK_NUMPAD1 );
		REGISTER_ENUM_STRING( VK_NUMPAD2 );
		REGISTER_ENUM_STRING( VK_NUMPAD3 );
		REGISTER_ENUM_STRING( VK_NUMPAD4 );
		REGISTER_ENUM_STRING( VK_NUMPAD5 );
		REGISTER_ENUM_STRING( VK_NUMPAD6 );
		REGISTER_ENUM_STRING( VK_NUMPAD7 );
		REGISTER_ENUM_STRING( VK_NUMPAD8 );
		REGISTER_ENUM_STRING( VK_NUMPAD9 );
		REGISTER_ENUM_STRING( VK_MULTIPLY );
		REGISTER_ENUM_STRING( VK_ADD );
		REGISTER_ENUM_STRING( VK_SEPARATOR );
		REGISTER_ENUM_STRING( VK_SUBTRACT );
		REGISTER_ENUM_STRING( VK_DECIMAL );
		REGISTER_ENUM_STRING( VK_DIVIDE );
		REGISTER_ENUM_STRING( VK_F1 );
		REGISTER_ENUM_STRING( VK_F2 );
		REGISTER_ENUM_STRING( VK_F3 );
		REGISTER_ENUM_STRING( VK_F4 );
		REGISTER_ENUM_STRING( VK_F5 );
		REGISTER_ENUM_STRING( VK_F6 );
		REGISTER_ENUM_STRING( VK_F7 );
		REGISTER_ENUM_STRING( VK_F8 );
		REGISTER_ENUM_STRING( VK_F9 );
		REGISTER_ENUM_STRING( VK_F10 );
		REGISTER_ENUM_STRING( VK_F11 );
		REGISTER_ENUM_STRING( VK_F12 );
		REGISTER_ENUM_STRING( VK_F13 );
		REGISTER_ENUM_STRING( VK_F14 );
		REGISTER_ENUM_STRING( VK_F15 );
		REGISTER_ENUM_STRING( VK_F16 );
		REGISTER_ENUM_STRING( VK_F17 );
		REGISTER_ENUM_STRING( VK_F18 );
		REGISTER_ENUM_STRING( VK_F19 );
		REGISTER_ENUM_STRING( VK_F20 );
		REGISTER_ENUM_STRING( VK_F21 );
		REGISTER_ENUM_STRING( VK_F22 );
		REGISTER_ENUM_STRING( VK_F23 );
		REGISTER_ENUM_STRING( VK_F24 );
		REGISTER_ENUM_STRING( VK_NUMLOCK );
		REGISTER_ENUM_STRING( VK_SCROLL );
		REGISTER_ENUM_STRING( VK_OEM_NEC_EQUAL );
		REGISTER_ENUM_STRING( VK_OEM_1 );
		REGISTER_ENUM_STRING( VK_OEM_2 );
		REGISTER_ENUM_STRING( VK_OEM_3 );
		REGISTER_ENUM_STRING( VK_OEM_4 );
		REGISTER_ENUM_STRING( VK_OEM_5 );
		REGISTER_ENUM_STRING( VK_OEM_6 );
		REGISTER_ENUM_STRING( VK_OEM_7 );
		REGISTER_ENUM_STRING( VK_OEM_PLUS );
		REGISTER_ENUM_STRING( VK_OEM_COMMA );
		REGISTER_ENUM_STRING( VK_OEM_MINUS );
		REGISTER_ENUM_STRING( VK_OEM_PERIOD );
		REGISTER_ENUM_STRING( VK_LSHIFT );
		REGISTER_ENUM_STRING( VK_RSHIFT );
		REGISTER_ENUM_STRING( VK_LCONTROL );
		REGISTER_ENUM_STRING( VK_RCONTROL );
	}

	bool IsVkDown( int32 vk )
	{
		return ( GetKeyState( vk ) & 0x8000 ) > 0;
	}

	float GetMouseZAxis( const MSG& wndMsg )
	{
		switch ( wndMsg.message )
		{
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
			return -1.0f;
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
			return 1.0f;
		case WM_MOUSEWHEEL:
			return static_cast<float>( GET_WHEEL_DELTA_WPARAM( wndMsg.wParam ) ) / WHEEL_DELTA;
		default:
			return 0.f;
		}
	}
}

namespace engine
{
	bool WindowPlatformInputConvertor::Initialize()
	{
		return m_inputMap.Initialize();
	}

	void WindowPlatformInputConvertor::ProcessInput( logic::ILogic& logic, const MSG& wndMsg )
	{
		UserInput input;

		HWND hWnd = wndMsg.hwnd;
		uint32 message = wndMsg.message;
		LPARAM lParam = wndMsg.lParam;
		WPARAM wParam = wndMsg.wParam;

		switch ( message )
		{
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
		{
			bool bIsKeyDown = ( message == WM_KEYDOWN ) || ( message == WM_SYSKEYDOWN );

			uint32 vk = static_cast<uint32>( wParam );
			if ( vk == VK_SHIFT )
			{
				if ( IsVkDown( VK_LSHIFT ) == bIsKeyDown )
				{
					input.m_code = UIC_LEFTSHIFT;
				}

				if ( IsVkDown( VK_RSHIFT ) == bIsKeyDown )
				{
					input.m_code = UIC_RIGHTSHIFT;
				}
			}
			else if ( vk == VK_CONTROL )
			{
				if ( IsVkDown( VK_LCONTROL ) == bIsKeyDown )
				{
					input.m_code = UIC_LEFTCONTROL;
				}

				if ( IsVkDown( VK_RCONTROL ) == bIsKeyDown )
				{
					input.m_code = UIC_RIGHTCONTROL;
				}
			}
			else if ( vk == VK_MENU )
			{
				if ( IsVkDown( VK_LMENU ) == bIsKeyDown )
				{
					input.m_code = UIC_LEFTALT;
				}

				if ( IsVkDown( VK_RMENU ) == bIsKeyDown )
				{
					input.m_code = UIC_RIGHTALT;
				}
			}
			else
			{
				input.m_code = ConvertToUserInputCode( vk );
				if ( input.m_code == UIC_UNKNOWN )
				{
					return;
				}
			}

			input.m_axis[UserInput::Z_AXIS] = ( message == WM_KEYDOWN ) ? -1.f : 1.f;
			m_buttonStates.SetButtonState( input.m_code, message == WM_KEYDOWN );
		}
		break;
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_MOUSEWHEEL:
		case WM_MOUSEMOVE:
		{
			input.m_code = ConvertToUserInputCode( message );
			if ( input.m_code == UIC_UNKNOWN )
			{
				return;
			}

			POINT pt{ GET_X_LPARAM( lParam ),  GET_Y_LPARAM( lParam ) };
			if ( message == WM_MOUSEWHEEL )
			{
				ScreenToClient( hWnd, &pt );
			}

			Vector2 curMousePos( static_cast<float>( pt.x ), static_cast<float>( pt.y ) );

			if ( message == WM_MOUSEMOVE )
			{
				m_prevMousePos = curMousePos - m_prevMousePos;
				input.m_axis[UserInput::X_AXIS] = m_prevMousePos.x;
				input.m_axis[UserInput::Y_AXIS] = m_prevMousePos.y;
			}
			else
			{
				input.m_axis[UserInput::X_AXIS] = curMousePos.x;
				input.m_axis[UserInput::Y_AXIS] = curMousePos.y;
			}
			input.m_axis[UserInput::Z_AXIS] = GetMouseZAxis( wndMsg );
			m_prevMousePos = curMousePos;

			bool bMouseButtonPressed = ( message == WM_LBUTTONDOWN )
				|| ( message == WM_RBUTTONDOWN )
				|| ( message == WM_MBUTTONDOWN );
			m_buttonStates.SetButtonState( input.m_code, bMouseButtonPressed );
		}
		break;
		case WM_CHAR:
			if ( IsWindowUnicode( hWnd ) )
			{
				if ( wParam > 0 && wParam < 0x10000 )
				{
					logic.HandleTextInput( wParam, true );
				}
			}
			else
			{
				wchar_t wch = 0;
				::MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, (char*)&wParam, 1, &wch, 1 );
				logic.HandleTextInput( wch, false );
			}
			return;
			break;
		default:
			//Message UnHandled;
			return;
			break;
		}

		logic.HandleUserInput( input );
	}

	UserInputCode WindowPlatformInputConvertor::ConvertToUserInputCode( uint32 msg )
	{
		return m_inputMap.Convert( msg );
	}

	bool WindowPlatformInputMap::Initialize()
	{
		RegisterWindowInputEnumString();
		m_userInputMap.Initialize();
		return m_userInputMap.LoadConfig( "./Configs/window_platform.json" );
	}
}
