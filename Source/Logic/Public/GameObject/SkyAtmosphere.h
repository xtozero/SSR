#pragma once

#include "GameObject.h"

namespace logic
{
	class SkyAtmosphereComponent;

	class SkyAtmosphere : public CGameObject
	{
		GENERATE_CLASS_TYPE_INFO( SkyAtmosphere )

	public:
		virtual bool IgnorePicking() const override { return true; }

		SkyAtmosphere();

	private:
		SkyAtmosphereComponent* m_pSkyAtmosphereComponent = nullptr;
	};
}
