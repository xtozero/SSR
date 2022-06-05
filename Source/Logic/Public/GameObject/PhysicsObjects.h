#pragma once

#include "GameObject.h"

class SphereObject : public CGameObject
{
	GENERATE_CLASS_TYPE_INFO( SphereObject )

public:
	SphereObject();
};

class BoxObject : public CGameObject
{
	GENERATE_CLASS_TYPE_INFO( BoxObject )

public:
	BoxObject();
};
