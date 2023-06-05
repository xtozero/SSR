#pragma once

#include "common.h"
#include "SizedTypes.h"

#include <array>

class VolumetricFogComponent;

namespace rendercore
{
	class Scene;
	class VolumetricFogSceneInfo;

	class VolumetricFogProxy
	{
	public:
		const std::array<uint32, 3>& FrustumGridSize() const;
		float G() const;
		float UniformDensity() const;
		float Intensity() const;

		RENDERCORE_DLL VolumetricFogProxy( const VolumetricFogComponent& component );

	private:
		friend Scene;
		VolumetricFogSceneInfo* m_volumetricFogSceneInfo = nullptr;

		std::array<uint32, 3> m_frustumGridSize = { 160, 90, 128 };
		float m_g = 0.7f;
		float m_uniformDensity = 5.f;
		float m_intensity = 50.f;
	};
}