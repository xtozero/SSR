#include "SkyAtmosphereProxy.h"

SkyAtmospherePorxy::SkyAtmospherePorxy( const SkyAtmosphereComponent& component )
{
}

rendercore::SkyAtmosphereRenderSceneInfo*& SkyAtmospherePorxy::RenderSceneInfo()
{
	return m_renderSceneInfo;
}
