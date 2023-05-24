#pragma once

#include "common.h"

class VolumetricFogComponent;

namespace rendercore
{
	class Scene;
	class VolumetricFogSceneInfo;

	class VolumetricFogProxy
	{
	public:
		RENDERCORE_DLL VolumetricFogProxy( const VolumetricFogComponent& component );

	private:
		friend Scene;
		VolumetricFogSceneInfo* m_volumetricFogSceneInfo = nullptr;
	};
}