#pragma once

#include "EnumStringMap.h"
#include "UICMap.h"

#include <WinUser.h>

class WindowPlatformKeyMap
{
public:
	bool Initialize( )
	{
		REGISTER_ENUM_STRING( WM_MOUSEMOVE );
		REGISTER_ENUM_STRING( WM_LBUTTONDOWN );
		REGISTER_ENUM_STRING( WM_LBUTTONUP );
		REGISTER_ENUM_STRING( WM_RBUTTONDOWN );
		REGISTER_ENUM_STRING( WM_RBUTTONUP );
		REGISTER_ENUM_STRING( WM_MBUTTONDOWN );
		REGISTER_ENUM_STRING( WM_MBUTTONUP );
		REGISTER_ENUM_STRING( WM_MOUSEWHEEL );

		m_userInputMap.Initialize( );
		return m_userInputMap.LoadConfig( _T( "../cfg/window_platform.cfg" ) );
	}

	USER_INPUT_CODE Convert( unsigned long code )
	{
		return m_userInputMap.Convert( code );
	}

private:
	UICMap m_userInputMap;
};

