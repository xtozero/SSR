#pragma once

#include "RefHandle.h"
#include "Texture.h"

namespace rendercore
{
	class IScene;
	class RenderGraph;
	class RenderViewGroup;

	struct SSGIRenderParams
	{
		RefHandle<agl::Buffer> m_viewShaderArguments;

		RefHandle<agl::Texture> m_sceneColor;
		RefHandle<agl::Texture> m_viewSpaceDistance;
		RefHandle<agl::Texture> m_prevViewSpaceDistance;
		RefHandle<agl::Texture> m_worldNormal;
		RefHandle<agl::Texture> m_velocity;

		float m_thickness = 0.f;
		float m_viewSpaceRadius = 0.f;
		uint32 m_numSlices = 0;
		uint32 m_numSteps = 0;
		float m_colorIntensity = 1.f;

		int32 m_denoiseKernelRadius = 1;
	};

	class SSGIRenderPass
	{
	public:
		RefHandle<agl::Texture> Render( RenderGraph& renderGraph, const SSGIRenderParams& param );

	private:
		RefHandle<agl::Texture> m_prevSSGI = BlackTexture;
	};
}