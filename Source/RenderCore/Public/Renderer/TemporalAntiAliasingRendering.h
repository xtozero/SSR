#pragma once

#include "PassProcessor.h"
#include "SizedTypes.h"
#include "ShaderArguments.h"

namespace rendercore
{
	class IRendererRenderTargets;
	class RenderGraph;
	class RenderViewGroup;

	BEGIN_SHADER_ARGUMENTS_STRUCT( TAAParameters )
		DECLARE_VALUE( float, BlendWeight )
		DECLARE_RESOURCE( agl::ShaderResourceView, HistoryTex )
		DECLARE_RESOURCE( agl::SamplerState, HistoryTexSampler )
		DECLARE_RESOURCE( agl::ShaderResourceView, SceneTex )
		DECLARE_RESOURCE( agl::SamplerState, SceneTexSampler )
		DECLARE_RESOURCE( agl::ShaderResourceView, VelocityTex )
		DECLARE_RESOURCE( agl::SamplerState, VelocityTexSampler )
	END_SHADER_ARGUMENTS_STRUCT()

	class TAAResolveProcessor final : public IPassProcessor
	{
	protected:
		virtual std::optional<DrawSnapshot> ProcessInternal( const PrimitiveSubMesh& subMesh, const PassShader& passShader ) override;
		virtual PassShader CollectPassShader( MaterialResource& material ) const override;
	};

	class TAARenderer final
	{
	public:
		void Render( RenderGraph& renderGraph, IRendererRenderTargets& renderTargets, RenderViewGroup& renderViewGroup );
		void Resovle( RenderGraph& renderGraph, IRendererRenderTargets& renderTargets, RenderViewGroup& renderViewGroup );
		void UpdateHistory( RenderGraph& renderGraph, IRendererRenderTargets& renderTargets, RenderViewGroup& renderViewGroup );

		TAARenderer();

	private:
		RefHandle<ShaderArguments> m_shaderArguments;
		TAAParameters m_parameters;
		bool m_paramUploaded = false;
	};
}