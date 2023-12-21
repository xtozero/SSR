#pragma once

#include "Buffer.h"
#include "GraphicsApiResource.h"
#include "Texture.h"

namespace rendercore
{
	class LightSceneInfo;

	struct RSMTextures
	{
		agl::RefHandle<agl::Texture> m_worldPosition;
		agl::RefHandle<agl::Texture> m_normal;
		agl::RefHandle<agl::Texture> m_flux;
	};

	struct LightInjectionParameters
	{
		const LightSceneInfo* lightInfo = nullptr;
		agl::RefHandle<agl::Buffer> m_sceneViewParameters;
		agl::RefHandle<agl::Buffer> m_shadowDepthPassParameters;
		RSMTextures m_rsmTextures;
	};

	struct LPVTextures
	{
		agl::RefHandle<agl::Texture> m_coeffR;
		agl::RefHandle<agl::Texture> m_coeffG;
		agl::RefHandle<agl::Texture> m_coeffB;
	};

	class LightPropagationVolume
	{
	public:
		void Prepare();
		void AddLight( const LightInjectionParameters& params );

	private:
		void InitResource();
		void ClearLPV();
		RSMTextures DownSampleRSMs( const LightSceneInfo& lightInfo, const RSMTextures& rsmTextures );
		void InjectToLPV( agl::Buffer* sceneViewParameters, agl::Buffer* shadowDepthPassParameters, const RSMTextures& downSampledTex );

		agl::RefHandle<agl::Buffer> m_lpvCommon;
		LPVTextures m_lpvTextures;
	};
}