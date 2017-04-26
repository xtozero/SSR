#include "stdafx.h"
#include "WindowPlatformInputConvertor.h"

#include "../Logic/ILogic.h"
#include "../shared/Util.h"

#include <windowsx.h>

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
			return GET_WHEEL_DELTA_WPARAM( wndMsg.wParam );
		default:
			return 0.f;
	}
}

bool WindowPlatformInputConvertor::Initialize( )
{
	return m_inputMap.Initialize( );
}

bool WindowPlatformInputConvertor::ProcessInput( ILogic& logic, const MSG& wndMsg )
{
	UserInput input;

	switch ( wndMsg.message )
	{
	case WM_KEYDOWN:
	case WM_KEYUP:
		{
			input = Convert( static_cast<unsigned long>( wndMsg.wParam ) );
			if ( input.m_code == UIC_UNKNOWN )
			{
				return false;
			}

			input.m_axis[UserInput::Z_AXIS] = ( wndMsg.message == WM_KEYDOWN ) ? -1.f : 1.f;
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
			input = Convert( static_cast<unsigned long>( wndMsg.message ) );
			if ( input.m_code == UIC_UNKNOWN )
			{
				return false;
			}

			CXMFLOAT2 curMousePos = { static_cast<float>( GET_X_LPARAM( wndMsg.lParam ) ),
									static_cast<float>( GET_Y_LPARAM( wndMsg.lParam ) ) };

			if ( wndMsg.message == WM_MOUSEMOVE )
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
		}
		break;
	default:
		//Message UnHandled;
		return false;
		break;
	}

	logic.HandleUserInput( input );
	return true;
}

UserInput WindowPlatformInputConvertor::Convert( unsigned long msg )
{
	UserInput input;
	input.m_code = m_inputMap.Convert( msg );

	return input;
}