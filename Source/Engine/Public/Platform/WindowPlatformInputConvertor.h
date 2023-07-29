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

	UserInputCode Convert( uint32 code )
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
	void ProcessInput( ILogic& logic, const MSG& wndMsg );

private:
	UserInputCode ConvertToUserInputCode( uint32 msg );
	WindowPlatformInputMap m_inputMap;

	Vector2 m_prevMousePos;
	UserInputButtonStates m_buttonStates;
};