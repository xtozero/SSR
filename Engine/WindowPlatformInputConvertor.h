#pragma once

#include "EnumStringMap.h"
#include "UICMap.h"
#include "../shared/Math/CXMFloat.h"

#include <WinUser.h>

class ILogic;

class WindowPlatformInputMap
{
public:
	bool Initialize( );

	USER_INPUT_CODE Convert( unsigned long code )
	{
		return m_userInputMap.Convert( code );
	}

private:
	UICMap m_userInputMap;
};

class WindowPlatformInputConvertor
{
public:
	bool Initialize( );
	bool ProcessInput( ILogic& logic, const MSG& wndMsg );

private:
	UserInput Convert( unsigned long msg );
	WindowPlatformInputMap m_inputMap;

	CXMFLOAT2 m_prevMousePos = { -FLT_MAX, -FLT_MAX };
};