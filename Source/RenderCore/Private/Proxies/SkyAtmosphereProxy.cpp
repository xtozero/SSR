#include "SkyAtmosphereProxy.h"

namespace rendercore
{
	SkyAtmospherePorxy::SkyAtmospherePorxy( [[maybe_unused]] const logic::SkyAtmosphereComponent& component )
	{
	}

	SkyAtmosphereRenderSceneInfo*& SkyAtmospherePorxy::RenderSceneInfo()
	{
		return m_renderSceneInfo;
	}
}
