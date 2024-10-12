#pragma once

#include "GameObject.h"

namespace logic
{
	class StaticMeshGameObject : public CGameObject
	{
		GENERATE_CLASS_TYPE_INFO( StaticMeshGameObject )

	public:
		StaticMeshGameObject();
	};
}
