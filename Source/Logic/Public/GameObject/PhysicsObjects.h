#pragma once

#include "GameObject.h"

namespace logic
{
	class SphereObject : public GameObject
	{
		GENERATE_CLASS_TYPE_INFO( SphereObject )

	public:
		SphereObject();
	};

	class BoxObject : public GameObject
	{
		GENERATE_CLASS_TYPE_INFO( BoxObject )

	public:
		BoxObject();
	};
}
