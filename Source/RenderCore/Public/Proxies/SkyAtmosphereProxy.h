#pragma once

#include "LibraryTool/Common.h"

namespace logic
{
	class SkyAtmosphereComponent;
}

namespace rendercore
{
	class SkyAtmosphereRenderSceneInfo;

	class SkyAtmospherePorxy final
	{
	public:
		RENDERCORE_DLL explicit SkyAtmospherePorxy( const logic::SkyAtmosphereComponent& component );

		SkyAtmosphereRenderSceneInfo*& RenderSceneInfo();

	private:
		SkyAtmosphereRenderSceneInfo* m_renderSceneInfo = nullptr;
	};
}
