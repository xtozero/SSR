#pragma once

#include "EnumStringMap.h"
#include "Math/Vector2.h"
#include "SizedTypes.h"
#include "UserInput/UICMap.h"

#include <WinUser.h>

namespace logic
{
	class ILogic;
}

namespace engine
{
	class IPlatform;

	class WindowPlatformInputMap
	{
	public:
		bool Initialize();

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
		bool Initialize( IPlatform& platform );
		bool ProcessInput( logic::ILogic& logic, const MSG& wndMsg );

	private:
		UserInputCode ConvertToUserInputCode( uint32 msg );

		IPlatform* m_windowPlatform = nullptr;

		WindowPlatformInputMap m_inputMap;

		Vector2 m_prevMousePos;
		UserInputButtonStates m_buttonStates;
	};
}
