#pragma once

#include "UserInput/UserInput.h"

namespace logic
{
	class CGameLogic;

	class IListener
	{
	public:
		virtual void ProcessInput( const engine::UserInput&, CGameLogic& gameLogic ) = 0;

		virtual ~IListener() = default;

	protected:
		IListener() = default;
	};
}