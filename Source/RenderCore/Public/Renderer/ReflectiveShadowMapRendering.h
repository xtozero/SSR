#pragma once

#include "Buffer.h"
#include "GraphicsApiResource.h"
#include "GraphicsPipelineState.h"
#include "ShaderArguments.h"
#include "SizedTypes.h"
#include "Texture.h"

namespace rendercore
{
	class RenderViewGroup;
	class RenderingShaderResource;
	class ShadowInfo;

	BEGIN_SHADER_ARGUMENTS_STRUCT( RSMsParameters )
		DECLARE_VALUE( uint32, NumSamplingPattern )
		DECLARE_VALUE( float, MaxRadius )
	END_SHADER_ARGUMENTS_STRUCT()

	struct RSMsRenderingParam final
	{
		RefHandle<agl::Texture> m_viewSpaceDistance;
		RefHandle<agl::Texture> m_worldNormal;
		
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

		RSMsRenderer();

	private:
		void AllocTextureForIndirectIllumination( const std::pair<uint32, uint32>& renderTargetSize );
		void CreateSamplingPattern();

		RefHandle<agl::Buffer> m_samplingPattern;
		RefHandle<ShaderArguments> m_shaderArguments;
		RefHandle<agl::Texture> m_indirectIllumination;
		SamplerState m_blackBorderSampler;
	};
}