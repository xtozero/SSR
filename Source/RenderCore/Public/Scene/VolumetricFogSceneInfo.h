#pragma once

#include "GraphicsApiResource.h"
#include "Scene/ShadowInfo.h"
#include "ShaderArguments.h"
#include "SizedTypes.h"
#include "Texture.h"

namespace rendercore
{
	class CommandList;
	class Scene;
	class VolumetricFogProxy;

	struct ForwardLightingResource;
	struct RenderView;

	BEGIN_SHADER_ARGUMENTS_STRUCT( VolumetricFogParameters )
		DECLARE_VALUE( float, Exposure )
		DECLARE_VALUE( float, DepthPackExponent )
		DECLARE_VALUE( float, NearPlaneDist )
		DECLARE_VALUE( float, FarPlaneDist )
	END_SHADER_ARGUMENTS_STRUCT()

	class VolumetricFogSceneInfo final
	{
	public:
		const VolumetricFogProxy* Proxy() const
		{
			return m_volumetricFogProxy;
		}

		VolumetricFogProxy*& Proxy()
		{
			return m_volumetricFogProxy;
		}

		RefHandle<agl::Texture> AccumulatedVolume() const
		{
			return m_accumulatedVolume;
		}

		ShaderArguments& GetShaderArguments()
		{
			return *m_shaderArguments.Get();
		}

		void CreateRenderData();
		void UpdateParameter();
		void PrepareFrustumVolume( Scene& scene, ForwardLightingResource& lightingResource, RenderThreadFrameData<ShadowInfo>& shadowInfos );

		VolumetricFogSceneInfo( VolumetricFogProxy* proxy );

	private:
		RefHandle<agl::Texture> FrustumVolume() const
		{
			return m_frustumVolume[m_numTick % std::extent_v<decltype( m_frustumVolume )>];
		}

		RefHandle<agl::Texture> HistoryVolume() const
		{
			return m_frustumVolume[( m_numTick + 1 ) % std::extent_v<decltype( m_frustumVolume )>];
		}

		void CreateVolumeTexture();
		void CalcInscattering( CommandList& commandList, Scene& scene, ForwardLightingResource& lightingResource, RenderThreadFrameData<ShadowInfo>& shadowInfos );
		void AccumulateScattering( CommandList& commandList );

		VolumetricFogProxy* m_volumetricFogProxy = nullptr;

		int64 m_numTick = -1;

		RefHandle<agl::Texture> m_frustumVolume[2];
		RefHandle<agl::Texture> m_accumulatedVolume;

		RefHandle<ShaderArguments> m_shaderArguments;

		bool m_needCreateRenderData = true;
		bool m_needUpdateParameter = true;
	};
}
