#pragma once

#include <memory>
#include <vector>

namespace logic
{
	class GameObject;
	class GameLogic;

	class WorldLoader
	{
	public:
		static bool Load( GameLogic& gameLogic, const char* worldAsset, size_t assetSize );
	};
}
