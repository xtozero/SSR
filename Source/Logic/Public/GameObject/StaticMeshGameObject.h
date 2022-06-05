#pragma once

#include "GameObject.h"

#include <string>

class CGameLogic;

class StaticMeshGameObject : public CGameObject
{
	GENERATE_CLASS_TYPE_INFO( StaticMeshGameObject )

public:
	StaticMeshGameObject();
};