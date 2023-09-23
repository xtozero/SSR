#include "SkyAtmosphereProxy.h"

namespace rendercore
{
	SkyAtmospherePorxy::SkyAtmospherePorxy( const logic::SkyAtmosphereComponent& component )
	{
	}

	SkyAtmosphereRenderSceneInfo*& SkyAtmospherePorxy::RenderSceneInfo()
	{
		return m_renderSceneInfo;
	}
}
