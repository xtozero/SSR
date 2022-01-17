#pragma once

#include "common.h"

class SkyAtmosphereComponent;
namespace rendercore
{
	class SkyAtmosphereRenderSceneInfo;
}

class SkyAtmospherePorxy
{
public:
	RENDERCORE_DLL SkyAtmospherePorxy( const SkyAtmosphereComponent& component );

	rendercore::SkyAtmosphereRenderSceneInfo*& RenderSceneInfo();

private:
	rendercore::SkyAtmosphereRenderSceneInfo* m_renderSceneInfo = nullptr;
};