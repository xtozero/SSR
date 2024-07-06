#pragma once

#include "PassProcessor.h"
#include "SizedTypes.h"
#include "ShaderArguments.h"

namespace rendercore
{
	class IRendererRenderTargets;
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
	public:
		virtual std::optional<DrawSnapshot> Process( const PrimitiveSubMesh& subMesh ) override;
	};

	class TAARenderer final
	{
	public:
		void Render( IRendererRenderTargets& renderTargets, RenderViewGroup& renderViewGroup );
		void Resovle( IRendererRenderTargets& renderTargets, RenderViewGroup& renderViewGroup );
		void UpdateHistory( IRendererRenderTargets& renderTargets, RenderViewGroup& renderViewGroup );

		TAARenderer();

	private:
		RefHandle<ShaderArguments> m_shaderArguments;
		TAAParameters m_parameters;
		bool m_paramUploaded = false;
	};
}