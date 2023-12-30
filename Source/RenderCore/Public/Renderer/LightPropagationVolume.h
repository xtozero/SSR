#pragma once

#include "Buffer.h"
#include "GraphicsApiResource.h"
#include "Scene/ShadowInfo.h"
#include "Texture.h"

#include <array>

namespace rendercore
{
	class LightSceneInfo;
	class RenderViewGroup;
	class RenderingShaderResource;

	struct LpvRSMTextures
	{
		agl::RefHandle<agl::Texture> m_worldPosition;
		agl::RefHandle<agl::Texture> m_normal;
		agl::RefHandle<agl::Texture> m_flux;
	};

	struct LpvLightInjectionParameters
	{
		const LightSceneInfo* lightInfo = nullptr;
		agl::RefHandle<agl::Buffer> m_sceneViewParameters;
		agl::RefHandle<agl::Buffer> m_shadowDepthPassParameters;
		LpvRSMTextures m_rsmTextures;
		std::array<float, CascadeShadowSetting::MAX_CASCADE_NUM> m_surfelAreas;
	};

	struct LpvRenderingParameters
	{
		agl::RefHandle<agl::Texture> m_viewSpaceDistance;
		agl::RefHandle<agl::Texture> m_worldNormal;
	};

	class LightPropagationVolume
	{
	public:
		void Prepare( const RenderViewGroup& renderViewGroup );
		void InjectLight( const LpvLightInjectionParameters& params );
		void Propagate();
		void Render( const LpvRenderingParameters& param, RenderingShaderResource& outRenderingShaderResource );

	private:
		struct LPVTextures
		{
			agl::RefHandle<agl::Texture> m_coeffR;
			agl::RefHandle<agl::Texture> m_coeffG;
			agl::RefHandle<agl::Texture> m_coeffB;
			agl::RefHandle<agl::Texture> m_coeffOcclusion;
		};

		void AllocTextureForIndirectIllumination( const std::pair<uint32, uint32>& renderTargetSize );
		LPVTextures AllocVolumeTextures( bool allocForOcclusion );
		void InitResource( const std::pair<uint32, uint32>& renderTargetSize );
		void ClearLPV();
		LpvRSMTextures DownSampleRSMs( const LightSceneInfo& lightInfo, const LpvRSMTextures& rsmTextures );
		void InjectToLPV( const LpvLightInjectionParameters& params, const LpvRSMTextures& downSampledTex );

		agl::RefHandle<agl::Buffer> m_lpvCommon;

		LPVTextures m_lpvTextures;

		agl::RefHandle<agl::Texture> m_indirectIllumination;
	};
}