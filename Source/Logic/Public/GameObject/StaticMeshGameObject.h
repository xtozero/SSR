#pragma once

#include "GameObject.h"

#include <string>

namespace logic
{
	class CGameLogic;

	class StaticMeshGameObject : public CGameObject
	{
		GENERATE_CLASS_TYPE_INFO( StaticMeshGameObject )

	public:
		StaticMeshGameObject();
	};
}
