#include "TemporalAntiAliasingRendering.h"

#include "CommandList.h"
#include "CommonRenderResource.h"
#include "GlobalShaders.h"
#include "GraphicsApiResource.h"
#include "RenderGraph.h"
#include "RenderView.h"
#include "Scene/IScene.h"
#include "Scene/PrimitiveSceneInfo.h"
#include "SceneRenderer.h"
#include "StaticState.h"
#include "VertexCollection.h"
#include "Viewport.h"

namespace rendercore
{
	class TAAResolvePS final : public GlobalShaderBase<PixelShader, TAAResolvePS>
	{};

	REGISTER_GLOBAL_SHADER( TAAResolvePS, "TemporalAntiAliasing/PS_TAAResolve.fx", "main" );

	std::optional<DrawSnapshot> TAAResolveProcessor::ProcessInternal( const PrimitiveSubMesh& subMesh, const PassShader& passShader )
	{
		DepthStencilOption depthStencilOption;
		depthStencilOption.m_depth.m_enable = false;
		depthStencilOption.m_stencil.m_enable = false;

		PassRenderOption passRenderOption = {
			.m_depthStencilOption = &depthStencilOption
		};

		return BuildDrawSnapshot( subMesh, passShader, passRenderOption, VertexStreamLayoutType::Default );
	}

	PassShader TAAResolveProcessor::CollectPassShader( [[maybe_unused]] MaterialResource& material ) const
	{
		PassShader passShader = {
			.m_vertexShader = FullScreenQuadVS(),
			.m_pixelShader = TAAResolvePS()
		};

		return passShader;
	}

	void TAARenderPass::Render( RenderGraph& renderGraph, IRendererRenderTargets& renderTargets, RenderViewGroup& renderViewGroup )
	{
		agl::Texture* sceneTex = renderTargets.GetSceneColor();
		agl::Texture* historyTex = renderTargets.GetTAAHistory();
		agl::Texture* resolveTex = renderTargets.GetTAAResolve();
		agl::Texture* velocityTex = renderTargets.GetVelocity();

		if ( sceneTex == nullptr
			|| historyTex == nullptr
			|| resolveTex == nullptr
			|| velocityTex == nullptr )
		{
			return;
		}

		IScene& scene = renderViewGroup.Scene();
		if ( scene.GetNumFrame() == 1 )
		{
			BEGIN_RG_RESOURCE_STRUCT( TAACopyPassResource )
				DECLARE_RG_TEXTURE_COPY_DEST( historyTex )
				DECLARE_RG_TEXTURE_COPY_SOURCE( sceneTex )
			END_RG_RESOURCE_STRUCT();

			auto rgHistoryTex = renderGraph.RegisterExternalResource( historyTex );
			auto rgSceneTex = renderGraph.RegisterExternalResource( sceneTex );

			TAACopyPassResource passResource = {
				.m_historyTex = rgHistoryTex,
				.m_sceneTex = rgSceneTex
			};

			renderGraph.AddPass(
				passResource,
				[passResource]( CopyCommandList& commandList )
				{
					auto historyTex = passResource.m_historyTex->Get();
					auto sceneTex = passResource.m_sceneTex->Get();

					commandList.CopyResource( historyTex, sceneTex, false );
				} );
		}
		else
		{
			Resovle( renderGraph, renderTargets, renderViewGroup );
			UpdateHistory( renderGraph, renderTargets, renderViewGroup );
		}
	}

	void TAARenderPass::Resovle( RenderGraph& renderGraph, IRendererRenderTargets& renderTargets, RenderViewGroup& renderViewGroup )
	{
		agl::Texture* historyTex = renderTargets.GetTAAHistory();
		agl::Texture* resolveTex = renderTargets.GetTAAResolve();
		agl::Texture* sceneTex = renderTargets.GetSceneColor();
		agl::Texture* velocityTex = renderTargets.GetVelocity();
		ShaderArguments& viewArguments = renderViewGroup.Scene().GetViewShaderArguments();

		auto rgResolveTex = renderGraph.RegisterExternalResource( resolveTex );

		// Linear Sampler
		SamplerState historyTexSampler = StaticSamplerState<>::Get();

		// Point Sampler
		SamplerState sceneTexSampler = StaticSamplerState<agl::TextureFilter::Point>::Get();

		// Point Sampler
		SamplerState velocityTexSampler = StaticSamplerState<agl::TextureFilter::Point>::Get();

		m_parameters.HistoryTex = historyTex->SRV();
		m_parameters.HistoryTexSampler = historyTexSampler.Resource();
		m_parameters.SceneTex = sceneTex->SRV();
		m_parameters.SceneTexSampler = sceneTexSampler.Resource();
		m_parameters.VelocityTex = velocityTex->SRV();
		m_parameters.VelocityTexSampler = velocityTexSampler.Resource();

		m_shaderArguments->Update( m_parameters );

		auto rgHistoryTex = renderGraph.RegisterExternalResource( historyTex );
		auto rgSceneTex = renderGraph.RegisterExternalResource( sceneTex );
		auto rgVelocityTex = renderGraph.RegisterExternalResource( velocityTex );

		BEGIN_RG_RESOURCE_STRUCT( ResolvePassResource )
			DECLARE_RG_TEXTURE_PIXEL_SRV( historyTex )
			DECLARE_RG_TEXTURE_PIXEL_SRV( sceneTex )
			DECLARE_RG_TEXTURE_PIXEL_SRV( velocityTex )
		END_RG_RESOURCE_STRUCT();

		ResolvePassResource resolvePassResource = {
			.m_historyTex = rgHistoryTex,
			.m_sceneTex = rgSceneTex,
			.m_velocityTex = rgVelocityTex
		};

		auto width = resolveTex->GetTrait().m_width;
		auto height = resolveTex->GetTrait().m_height;

		RasterOutput rasterOutput;
		rasterOutput.SetRenderTarget( 0, rgResolveTex, RasterOutputLoadAction::Clear );
		rasterOutput.SetViewport( width, height);
		rasterOutput.SetScissorRect( width, height );

		renderGraph.AddPass(
			resolvePassResource,
			rasterOutput,
			[this, &viewArguments]( CommandList& commandList )
			{
				TAAResolveProcessor resolveProcessor;
				auto result = resolveProcessor.Process( FullScreenQuadDrawInfo() );
				if ( result.has_value() == false )
				{
					return;
				}

				DrawSnapshot& snapshot = *result;

				ResourceBinder resourceBinder;
				resourceBinder.Add( &viewArguments );
				resourceBinder.Add( m_shaderArguments.Get() );

				GraphicsPipelineState& pipelineState = snapshot.m_pipelineState;
				resourceBinder.Bind( pipelineState.m_shaderState, snapshot.m_shaderBindings );

				AddSingleDrawPass( commandList, snapshot );
			});
	}

	void TAARenderPass::UpdateHistory( RenderGraph& renderGraph, IRendererRenderTargets& renderTargets, RenderViewGroup& renderViewGroup )
	{
		agl::Texture* historyTex = renderTargets.GetTAAHistory();
		agl::Texture* sceneTex = renderTargets.GetSceneColor();
		agl::Texture* resolveTex = renderTargets.GetTAAResolve();

		auto rgHistoryTex = renderGraph.RegisterExternalResource( historyTex );
		auto rgSceneTex = renderGraph.RegisterExternalResource( sceneTex );
		auto rgResolveTex = renderGraph.RegisterExternalResource( resolveTex );

		BEGIN_RG_RESOURCE_STRUCT( UpdateHistoryPassResource )
			DECLARE_RG_TEXTURE_COPY_DEST( historyTex )
			DECLARE_RG_TEXTURE_COPY_DEST( sceneTex )
			DECLARE_RG_TEXTURE_COPY_SOURCE( resolveTex )
		END_RG_RESOURCE_STRUCT();

		UpdateHistoryPassResource passResource = {
			.m_historyTex = rgHistoryTex,
			.m_sceneTex = rgSceneTex,
			.m_resolveTex = rgResolveTex
		};

		renderGraph.AddPass(
			passResource,
			[passResource]( CopyCommandList& commandList )
			{
				auto historyTex = passResource.m_historyTex->Get();
				auto sceneTex = passResource.m_sceneTex->Get();
				auto resolveTex = passResource.m_resolveTex->Get();

				commandList.CopyResource( historyTex, resolveTex, false );
				commandList.CopyResource( sceneTex, resolveTex, false );
			} );
	}

	TAARenderPass::TAARenderPass()
	{
		m_shaderArguments = TAAParameters::CreateShaderArguments();
		m_parameters.BlendWeight = 0.9f;
	}
}