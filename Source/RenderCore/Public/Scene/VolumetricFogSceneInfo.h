#pragma once

#include "GraphicsApiResource.h"
#include "Scene/ShadowInfo.h"
#include "SizedTypes.h"
#include "Texture.h"

namespace rendercore
{
	class CommandList;
	class Scene;
	class VolumetricFogProxy;

	struct RenderView;

	struct VolumetricFogParameter
	{
		float m_exposure = 0.4f;
		float m_depthPackExponent = 2.f;
		float m_nearPlaneDist = 1.f;
		float m_farPlaneDist = 300.f;
	};

	class VolumetricFogSceneInfo
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

		agl::RefHandle<agl::Texture> AccumulatedVolume() const
		{
			return m_accumulatedVolume;
		}

		TypedConstatBuffer<VolumetricFogParameter>& GetVolumetricFogParameter()
		{
			return m_volumetricFogParameter;
		}

		void CreateRenderData();
		void UpdateParameter();
		void PrepareFrustumVolume( Scene& scene, RenderView& renderView, RenderThreadFrameData<ShadowInfo>& shadowInfos );

		VolumetricFogSceneInfo( VolumetricFogProxy* proxy );

	private:
		agl::RefHandle<agl::Texture> FrustumVolume() const
		{
			return m_frustumVolume[m_numTick % std::extent_v<decltype( m_frustumVolume )>];
		}

		agl::RefHandle<agl::Texture> HistoryVolume() const
		{
			return m_frustumVolume[( m_numTick + 1 ) % std::extent_v<decltype( m_frustumVolume )>];
		}

		void CreateVolumeTexture();
		void CalcInscattering( CommandList& commandList, Scene& scene, RenderView& renderView, RenderThreadFrameData<ShadowInfo>& shadowInfos );
		void AccumulateScattering( CommandList& commandList );

		VolumetricFogProxy* m_volumetricFogProxy = nullptr;

		int64 m_numTick = -1;

		agl::RefHandle<agl::Texture> m_frustumVolume[2];
		agl::RefHandle<agl::Texture> m_accumulatedVolume;

		TypedConstatBuffer<VolumetricFogParameter> m_volumetricFogParameter;

		bool m_needCreateRenderData = true;
		bool m_needUpdateParameter = true;
	};
}
