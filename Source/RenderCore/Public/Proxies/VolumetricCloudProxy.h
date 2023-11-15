#pragma once

#include "ColorTypes.h"
#include "common.h"
#include "Math/Vector4.h"

namespace logic
{
	class VolumetricCloudComponent;
}

namespace rendercore
{
	class Scene;
	class VolumetricCloudSceneInfo;

	class VolumetricCloudProxy final
	{
	public:
		RENDERCORE_DLL VolumetricCloudProxy( const logic::VolumetricCloudComponent& component );

		float EarthRadius() const;
		float InnerRadius() const;
		float OuterRadius() const;
		float LightAbsorption() const;
		float DensityScale() const;
		const ColorF& CloudColor() const;
		float Crispiness() const;
		float Curliness() const;
		float DensityFactor() const;

	private:
		friend Scene;
		VolumetricCloudSceneInfo* m_volumetricCloudSceneInfo = nullptr;

		float m_earthRadius = 0.f;
		float m_innerRadius = 0.f;
		float m_outerRadius = 0.f;
		float m_lightAbsorption = 0.0035f;
		float m_densityScale = 0.45f;
		ColorF m_cloudColor = { 0.38235f, 0.41176f, 0.47059f, 1.f };
		float m_crispiness = 40.f;
		float m_curliness = 0.1f;
		float m_densityFactor = 0.2f;
	};
}
