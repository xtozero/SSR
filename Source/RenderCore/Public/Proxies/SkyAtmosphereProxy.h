#pragma once

#include "common.h"

namespace logic
{
	class SkyAtmosphereComponent;
}

namespace rendercore
{
	class SkyAtmosphereRenderSceneInfo;

	class SkyAtmospherePorxy
	{
	public:
		RENDERCORE_DLL SkyAtmospherePorxy( const logic::SkyAtmosphereComponent& component );

		SkyAtmosphereRenderSceneInfo*& RenderSceneInfo();

	private:
		SkyAtmosphereRenderSceneInfo* m_renderSceneInfo = nullptr;
	};
}
