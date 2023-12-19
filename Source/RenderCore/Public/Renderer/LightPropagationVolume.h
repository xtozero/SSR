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

	class LightPropagationVolume
	{
	public:
		void AddLight( const LightSceneInfo& lightInfo, const RSMTextures& rsmTextures );

	private:
		void InitResource();
		RSMTextures DownSampleRSMs( const LightSceneInfo& lightInfo, const RSMTextures& rsmTextures );

		agl::RefHandle<agl::Buffer> m_lpvCommon;
	};
}