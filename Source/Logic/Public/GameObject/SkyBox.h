#pragma once

#include "GameObject.h"

namespace logic
{
	class TexturedSkyComponent;

	class SkyBox : public GameObject
	{
		GENERATE_CLASS_TYPE_INFO( SkyBox )

	public:
		SkyBox();

	private:
		TexturedSkyComponent* m_pTexturedSkyComponent = nullptr;
	};
}
