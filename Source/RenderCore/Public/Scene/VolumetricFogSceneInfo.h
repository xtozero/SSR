#pragma once

#include "GraphicsApiResource.h"
#include "Scene/ShadowInfo.h"
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
		float padding[3];
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

		agl::RefHandle<agl::Texture> ScatteringTex() const
		{
			return m_frustumVolume;
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
		void CreateVolumeTexture();
		void CalcInscattering( CommandList& commandList, Scene& scene, RenderView& renderView, RenderThreadFrameData<ShadowInfo>& shadowInfos );
		void AccumulateScattering( CommandList& commandList );

		VolumetricFogProxy* m_volumetricFogProxy = nullptr;

		agl::RefHandle<agl::Texture> m_frustumVolume;

		TypedConstatBuffer<VolumetricFogParameter> m_volumetricFogParameter;

		bool m_needCreateRenderData = true;
		bool m_needUpdateParameter = true;
	};
}
