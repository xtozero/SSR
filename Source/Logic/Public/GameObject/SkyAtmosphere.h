#pragma once

#include "GameObject.h"

namespace logic
{
	class SkyAtmosphereComponent;

	class SkyAtmosphere : public GameObject
	{
		GENERATE_CLASS_TYPE_INFO( SkyAtmosphere )

	public:
		SkyAtmosphere();

	private:
		SkyAtmosphereComponent* m_pSkyAtmosphereComponent = nullptr;
	};
}
