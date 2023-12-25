#pragma once

#include "Buffer.h"
#include "GraphicsApiResource.h"
#include "Texture.h"

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
		void AddLight( const LpvLightInjectionParameters& params );
		void Propagate();
		void Render( const LpvRenderingParameters& param, RenderingShaderResource& outRenderingShaderResource );

	private:
		void AllocTextureForIndirectIllumination( const std::pair<uint32, uint32>& renderTargetSize );
		void InitResource( const std::pair<uint32, uint32>& renderTargetSize );
		void ClearLPV();
		LpvRSMTextures DownSampleRSMs( const LightSceneInfo& lightInfo, const LpvRSMTextures& rsmTextures );
		void InjectToLPV( agl::Buffer* sceneViewParameters, agl::Buffer* shadowDepthPassParameters, const LpvRSMTextures& downSampledTex );

		agl::RefHandle<agl::Buffer> m_lpvCommon;

		struct LPVTextures
		{
			agl::RefHandle<agl::Texture> m_coeffR;
			agl::RefHandle<agl::Texture> m_coeffG;
			agl::RefHandle<agl::Texture> m_coeffB;
		} m_lpvTextures;

		agl::RefHandle<agl::Texture> m_indirectIllumination;
	};
}