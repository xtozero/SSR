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
		agl::Texture* velocityTex = renderTargets.GetVelocity();

		if ( ( sceneTex == nullptr ) || ( velocityTex == nullptr ) )
		{
			return;
		}

		AllocateHistoryTexture( renderGraph, renderViewGroup );

		IScene& scene = renderViewGroup.Scene();
		if ( scene.GetNumFrame() == 1 )
		{
			BEGIN_RG_RESOURCE_STRUCT( TAACopyPassResource )
				DECLARE_RG_TEXTURE_COPY_DEST( historyTex )
				DECLARE_RG_TEXTURE_COPY_SOURCE( sceneTex )
			END_RG_RESOURCE_STRUCT();

			auto rgHistoryTex = renderGraph.RegisterExternalResource( m_historyTex.Get() );
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
			agl::TextureDesc desc = {
				.m_width = sceneTex->GetDesc().m_width,
				.m_height = sceneTex->GetDesc().m_height,
				.m_depth = 1,
				.m_sampleCount = 1,
				.m_sampleQuality = 0,
				.m_mipLevels = 1,
				.m_format = agl::ResourceFormat::R8G8B8A8_UNORM,
				.m_access = agl::ResourceAccess::Default,
				.m_bindType = agl::ResourceBindType::RenderTarget | agl::ResourceBindType::ShaderResource,
				.m_miscFlag = agl::ResourceMisc::None,
				.m_clearValue = agl::ResourceClearValue{
					.m_color = { 0.f, 0.f, 0.f, 0.f }
				}
			};

			auto rgResolveTex = renderGraph.CreateTexture( desc, "TAA.Resolve" );

			Resovle( renderGraph, renderTargets, rgResolveTex, renderViewGroup );
			UpdateHistory( renderGraph, renderTargets, rgResolveTex );
		}
	}

	TAARenderPass::TAARenderPass()
	{
		m_shaderArguments = TAAParameters::CreateShaderArguments();
		m_parameters.BlendWeight = 0.9f;
	}

	void TAARenderPass::AllocateHistoryTexture( RenderGraph& renderGraph, RenderViewGroup& renderViewGroup )
	{
		std::pair<uint32, uint32> rendertargetSize = renderViewGroup.GetViewport().SizeOnRenderThread();
		if ( m_historyTex.Get() != nullptr )
		{
			auto historyTexSize = std::make_pair( m_historyTex->GetDesc().m_width, m_historyTex->GetDesc().m_height );

			if ( rendertargetSize == historyTexSize )
			{
				return;
			}
		}

		agl::TextureDesc desc = {
			.m_width = rendertargetSize.first,
			.m_height = rendertargetSize.second,
			.m_depth = 1,
			.m_sampleCount = 1,
			.m_sampleQuality = 0,
			.m_mipLevels = 1,
			.m_format = agl::ResourceFormat::R8G8B8A8_UNORM,
			.m_access = agl::ResourceAccess::Default,
			.m_bindType = agl::ResourceBindType::RenderTarget | agl::ResourceBindType::ShaderResource,
			.m_miscFlag = agl::ResourceMisc::None
		};

		auto rgHistory = renderGraph.CreateTexture( desc, "TAA.History" );
		m_historyTex = renderGraph.ConvertToExternalResource( rgHistory );
	}

	void TAARenderPass::Resovle( RenderGraph& renderGraph, IRendererRenderTargets& renderTargets, RenderGraphTexture* rgResolveTex, RenderViewGroup& renderViewGroup )
	{
		agl::Texture* sceneTex = renderTargets.GetSceneColor();
		agl::Texture* velocityTex = renderTargets.GetVelocity();
		ShaderArguments& viewArguments = renderViewGroup.Scene().GetViewShaderArguments();

		// Linear Sampler
		SamplerState historyTexSampler = StaticSamplerState<>::Get();

		// Point Sampler
		SamplerState sceneTexSampler = StaticSamplerState<agl::TextureFilter::Point>::Get();

		// Point Sampler
		SamplerState velocityTexSampler = StaticSamplerState<agl::TextureFilter::Point>::Get();

		m_parameters.HistoryTex = m_historyTex->SRV();
		m_parameters.HistoryTexSampler = historyTexSampler.Resource();
		m_parameters.SceneTex = sceneTex->SRV();
		m_parameters.SceneTexSampler = sceneTexSampler.Resource();
		m_parameters.VelocityTex = velocityTex->SRV();
		m_parameters.VelocityTexSampler = velocityTexSampler.Resource();

		m_shaderArguments->Update( m_parameters );

		auto rgHistoryTex = renderGraph.RegisterExternalResource( m_historyTex.Get() );
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

		int32 width = sceneTex->GetDesc().m_width;
		int32 height = sceneTex->GetDesc().m_height;

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

	void TAARenderPass::UpdateHistory( RenderGraph& renderGraph, IRendererRenderTargets& renderTargets, RenderGraphTexture* rgResolveTex ) const
	{
		agl::Texture* sceneTex = renderTargets.GetSceneColor();

		auto rgHistoryTex = renderGraph.RegisterExternalResource( m_historyTex.Get() );
		auto rgSceneTex = renderGraph.RegisterExternalResource( sceneTex );

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
}
