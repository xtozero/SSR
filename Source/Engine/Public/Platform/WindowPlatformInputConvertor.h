#pragma once

#include "EnumStringMap.h"
#include "Math/Vector2.h"
#include "SizedTypes.h"
#include "UICMap.h"

#include <WinUser.h>

class ILogic;

class WindowPlatformInputMap
{
public:
	bool Initialize( );

	USER_INPUT_CODE Convert( uint32 code )
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
	UserInput Convert( uint32 msg );
	WindowPlatformInputMap m_inputMap;

	Vector2 m_prevMousePos;
};