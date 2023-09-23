#pragma once

#include "common.h"

#include <memory>
#include <vector>

namespace logic
{
	class CGameObject;
	class CGameLogic;

	class CWorldLoader
	{
	public:
		static bool Load( CGameLogic& gameLogic, const char* worldAsset, size_t assetSize );
	};
}
