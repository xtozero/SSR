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

		void CreateRenderData();
		void PrepareFrustumVolume( Scene& scene, RenderView& renderView, RenderThreadFrameData<ShadowInfo>& shadowInfos );

		VolumetricFogSceneInfo( VolumetricFogProxy* proxy );

	private:
		void CreateVolumeTexture();
		void CalcInscattering( CommandList& commandList, Scene& scene, RenderView& renderView, RenderThreadFrameData<ShadowInfo>& shadowInfos );
		void AccumulateScattering( CommandList& commandList );

		VolumetricFogProxy* m_volumetricFogProxy = nullptr;

		agl::RefHandle<agl::Texture> m_frustumVolume;

		bool m_needCreateRenderData = true;
	};
}
