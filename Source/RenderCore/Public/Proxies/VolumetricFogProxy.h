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
		float DepthPackExponent() const;
		float NearPlaneDist() const;
		float FarPlaneDist() const;
		float ShadowBias() const;

		RENDERCORE_DLL VolumetricFogProxy( const VolumetricFogComponent& component );

	private:
		friend Scene;
		VolumetricFogSceneInfo* m_volumetricFogSceneInfo = nullptr;

		std::array<uint32, 3> m_frustumGridSize = { 160, 90, 128 };
		float m_g = 0.7f;
		float m_uniformDensity = 5.f;
		float m_intensity = 50.f;
		float m_depthPackExponent = 2.f;
		float m_nearPlaneDist = 1.f;
		float m_farPlaneDist = 300.f;
		float m_shadowBias = 0.0015f;
	};
}