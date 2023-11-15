#pragma once

#include "Buffer.h"
#include "GraphicsApiResource.h"
#include "GraphicsPipelineState.h"
#include "SizedTypes.h"
#include "Texture.h"
#include "TypedBuffer.h"

namespace rendercore
{
	class RenderViewGroup;
	class RenderingShaderResource;
	class ShadowInfo;

	struct RSMsRenderingParam final
	{
		agl::RefHandle<agl::Texture> m_viewSpaceDistance;
		agl::RefHandle<agl::Texture> m_worldNormal;
		
		ShadowInfo* m_shadowInfos = nullptr;
		int32 m_numShadowInfos = 0;
	};

	struct RSMsConstantParameters final
	{
		uint32 m_numSamplingPattern;
		float m_maxRadius;
		float padding[2];
	};

	class RSMsRenderer final
	{
	public:
		void PreRender( const RenderViewGroup& renderViewGroup );
		void Render( const RSMsRenderingParam& param, RenderingShaderResource& outRenderingShaderResource );

	private:
		void AllocTextureForIndirectIllumination( const std::pair<uint32, uint32>& renderTargetSize );
		void CreateSamplingPattern();

		agl::RefHandle<agl::Buffer> m_samplingPattern;
		TypedConstatBuffer<RSMsConstantParameters> m_constantParams;
		agl::RefHandle<agl::Texture> m_indirectIllumination;
		SamplerState m_blackBorderSampler;
	};
}