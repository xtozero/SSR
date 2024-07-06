#pragma once

#include "GraphicsApiResource.h"
#include "Math/Vector.h"
#include "Math/Vector2.h"
#include "Math/Vector4.h"
#include "Texture.h"

namespace rendercore
{
	class VolumetricCloudProxy;

	BEGIN_SHADER_ARGUMENTS_STRUCT( VolumetricCloudRenderParameters )
		DECLARE_VALUE( Vector, SphereRadius )
		DECLARE_VALUE( float, LightAbsorption )
		DECLARE_VALUE( Vector4, LightPosOrDir )
		DECLARE_VALUE( ColorF, CloudColor )
		DECLARE_VALUE( Vector, WindDirection )
		DECLARE_VALUE( float, WindSpeed )
		DECLARE_VALUE( float, Crispiness )
		DECLARE_VALUE( float, Curliness )
		DECLARE_VALUE( float, DensityFactor )
		DECLARE_VALUE( float, DensityScale )
	END_SHADER_ARGUMENTS_STRUCT()

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

		ShaderArguments& GetShaderArguments()
		{
			return *m_shaderArguments.Get();
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
		RefHandle<ShaderArguments> m_shaderArguments;

		RefHandle<agl::Texture> m_baseCloudShape;
		RefHandle<agl::Texture> m_detailCloudShape;
		RefHandle<agl::Texture> m_weatherMap;

		bool m_needCreateRenderData = true;
	};
}
