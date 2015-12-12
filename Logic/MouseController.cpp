#include "stdafx.h"
#include "common.h"
#include "IListener.h"
#include "MouseController.h"
#include <windowsx.h>

MOUSE_INPUT_INFO CWinProcMouseInputTranslator::TranslateInput( const int message, const WPARAM wParam, const LPARAM lParam )
{
	MOUSE_INPUT_INFO inputInfo;

	inputInfo.m_x = GET_X_LPARAM( lParam );
	inputInfo.m_y = GET_Y_LPARAM( lParam );
	inputInfo.m_zDelta = GET_WHEEL_DELTA_WPARAM( wParam );

	switch ( message )
	{
	case WM_LBUTTONDOWN:
		inputInfo.m_type = MOUSE_INPUT_TYPE::L_DOWN;
		break;
	case WM_LBUTTONUP:
		inputInfo.m_type = MOUSE_INPUT_TYPE::L_UP;
		break;
	case WM_RBUTTONDOWN:
		inputInfo.m_type = MOUSE_INPUT_TYPE::R_DOWN;
		break;
	case WM_RBUTTONUP:
		inputInfo.m_type = MOUSE_INPUT_TYPE::R_UP;
		break;
	case WM_MBUTTONDOWN:
		inputInfo.m_type = MOUSE_INPUT_TYPE::M_DOWN;
		break;
	case WM_MBUTTONUP:
		inputInfo.m_type = MOUSE_INPUT_TYPE::M_UP;
		break;
	case WM_MOUSEMOVE:
		inputInfo.m_type = MOUSE_INPUT_TYPE::MOVE;
		break;
	case WM_MOUSEWHEEL:
		inputInfo.m_type = MOUSE_INPUT_TYPE::WHEEL_MOVE;
		break;
	default:
		inputInfo.m_type = MOUSE_INPUT_TYPE::UNKNOWN;
		break;
	}

	return inputInfo;
}

void CMouseController::ProcessInput( const MOUSE_INPUT_INFO& input )
{
	switch ( input.m_type )
	{
	case MOUSE_INPUT_TYPE::L_DOWN:
		FOR_EACH_VEC( m_listeners, iter )
		{
			( *iter )->OnLButtonDown( input.m_x, input.m_y );
		}
		break;
	case MOUSE_INPUT_TYPE::L_UP:
		FOR_EACH_VEC( m_listeners, iter )
		{
			( *iter )->OnLButtonUp( input.m_x, input.m_y );
		}
		break;
	case MOUSE_INPUT_TYPE::R_DOWN:
		FOR_EACH_VEC( m_listeners, iter )
		{
			( *iter )->OnRButtonDown( input.m_x, input.m_y );
		}
		break;
	case MOUSE_INPUT_TYPE::R_UP:
		FOR_EACH_VEC( m_listeners, iter )
		{
			( *iter )->OnRButtonUp( input.m_x, input.m_y );
		}
		break;
	case MOUSE_INPUT_TYPE::M_DOWN:
		FOR_EACH_VEC( m_listeners, iter )
		{
			( *iter )->OnMButtonDown( input.m_x, input.m_y );
		}
		break;
	case MOUSE_INPUT_TYPE::M_UP:
		FOR_EACH_VEC( m_listeners, iter )
		{
			( *iter )->OnMButtonUp( input.m_x, input.m_y );
		}
		break;
	case MOUSE_INPUT_TYPE::MOVE:
		FOR_EACH_VEC( m_listeners, iter )
		{
			( *iter )->OnMouseMove( input.m_x, input.m_y );
		}
		break;
	case MOUSE_INPUT_TYPE::WHEEL_MOVE:
		FOR_EACH_VEC( m_listeners, iter )
		{
			( *iter )->OnWheelMove( input.m_zDelta );
		}
		break;
	default:
	case MOUSE_INPUT_TYPE::UNKNOWN:
		DebugMsg( "UNKNOWN MOUSE INPUT TYPE\n" );
		return;
	}

	DebugMsg( "Mouse pos - ( %d, %d )\n", input.m_x, input.m_y );
}

void CMouseController::AddListener( IListener* listener )
{
	if ( listener )
	{
		m_listeners.push_back( listener );
	}
}