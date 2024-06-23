#pragma once

#include "GraphicsApiResource.h"
#include "Math/Vector.h"
#include "Math/Vector2.h"
#include "Math/Vector4.h"
#include "Texture.h"
#include "TypedBuffer.h"

namespace rendercore
{
	class VolumetricCloudProxy;

	struct VolumetricCloudRenderParameter final
	{
		Vector m_sphereRadius;
		float m_lightAbsorption = 0.0035f;
		Vector4 m_lightPosOrDir;
		ColorF m_cloudColor;
		Vector m_windDirection;
		float m_windSpeed = 0.f;
		float m_crispiness = 40.f;
		float m_curliness = 0.1f;
		float m_densityFactor = 0.2f;
		float m_densityScale = 0.45f;
	};

	class VolumetricCloudSceneInfo final
	{
	public:
		const VolumetricCloudProxy* Proxy() const
		{
			return m_cloudProxy;
		}

		VolumetricCloudProxy*& Proxy()
		{
			return m_cloudProxy;
		}

		TypedConstatBuffer<VolumetricCloudRenderParameter>& GetVolumetricCloudRenderParameter()
		{
			return m_renderParameter;
		}

		void CreateRenderData();

		agl::Texture* BaseCloudShape()
		{
			return m_baseCloudShape;
		}

		agl::Texture* DetailCloudShape()
		{
			return m_detailCloudShape;
		}

		agl::Texture* WeatherMap()
		{
			return m_weatherMap;
		}

		VolumetricCloudSceneInfo( VolumetricCloudProxy* proxy );

	private:
		void SetupCloudTexture();
		void GenerateWeatherMap();
		RefHandle<agl::Texture> CreateCloudTexture( uint32 texSize );

		VolumetricCloudProxy* m_cloudProxy = nullptr;
		TypedConstatBuffer<VolumetricCloudRenderParameter> m_renderParameter;

		RefHandle<agl::Texture> m_baseCloudShape;
		RefHandle<agl::Texture> m_detailCloudShape;
		RefHandle<agl::Texture> m_weatherMap;

		bool m_needCreateRenderData = true;
	};
}
