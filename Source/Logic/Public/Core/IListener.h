#pragma once

#include "UserInput/UserInput.h"

namespace logic
{
	class GameLogic;

	class IListener
	{
	public:
		virtual void ProcessInput( const engine::UserInput&, GameLogic& gameLogic ) = 0;

		virtual ~IListener() = default;

	protected:
		IListener() = default;
	};
}