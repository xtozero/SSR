#include "SkyAtmosphereProxy.h"

namespace rendercore
{
	SkyAtmospherePorxy::SkyAtmospherePorxy( const SkyAtmosphereComponent& component )
	{
	}

	SkyAtmosphereRenderSceneInfo*& SkyAtmospherePorxy::RenderSceneInfo()
	{
		return m_renderSceneInfo;
	}
}
