#include "TemporalAntiAliasingRendering.h"

#include "CommandList.h"
#include "CommonRenderResource.h"
#include "GlobalShaders.h"
#include "GraphicsApiResource.h"
#include "RenderView.h"
#include "Scene/IScene.h"
#include "Scene/PrimitiveSceneInfo.h"
#include "SceneRenderer.h"
#include "ShaderParameterUtils.h"
#include "TransitionUtils.h"
#include "VertexCollection.h"
#include "Viewport.h"

namespace rendercore
{
	class TAAResolvePS : public GlobalShaderCommon<PixelShader, TAAResolvePS>
	{};

	REGISTER_GLOBAL_SHADER( TAAResolvePS, "./Assets/Shaders/TemporalAntiAliasing/PS_TAAResolve.asset" );

	std::optional<DrawSnapshot> TAAResolveProcessor::Process( const PrimitiveSubMesh& subMesh )
	{
		PassShader passShader{
				FullScreenQuadVS(),
				nullptr,
				TAAResolvePS()
		};

		DepthStencilOption depthStencilOption;
		depthStencilOption.m_depth.m_enable = false;
		depthStencilOption.m_stencil.m_enable = false;

		PassRenderOption passRenderOption = {
			.m_depthStencilOption = &depthStencilOption
		};

		return BuildDrawSnapshot( subMesh, passShader, passRenderOption, VertexStreamLayoutType::Default );
	}

	void TAARenderer::Render( IRendererRenderTargets& renderTargets, RenderViewGroup& renderViewGroup )
	{
		agl::Texture* historyTex = renderTargets.GetTAAHistory();
		agl::Texture* resolveTex = renderTargets.GetTAAResolve();

		if ( historyTex == nullptr || resolveTex == nullptr )
		{
			return;
		}

		IScene& scene = renderViewGroup.Scene();
		if ( scene.GetNumFrame() == 1 )
		{
			agl::Texture* sceneTex = renderViewGroup.GetViewport().Texture();

			agl::ResourceTransition copyTransitions[] = {
				Transition( *historyTex, agl::ResourceState::CopyDest ),
				Transition( *sceneTex, agl::ResourceState::CopySource ),
			};
			GetCommandList().Transition( std::extent_v<decltype( copyTransitions )>, copyTransitions );

			GetCommandList().CopyResource( historyTex, sceneTex, true );

			agl::ResourceTransition rtTransition = Transition( *sceneTex, agl::ResourceState::RenderTarget );
			GetCommandList().Transition( 1, &rtTransition );
		}
		else
		{
			UploadParamToGpu();
			Resovle( renderTargets, renderViewGroup );
			UpdateHistory( renderTargets, renderViewGroup );
		}
	}

	void TAARenderer::UploadParamToGpu()
	{
		if ( m_paramUploaded == false )
		{
			m_resolveConstantBuffer.Update( m_resolveParam );
			m_paramUploaded = true;
		}
	}

	void TAARenderer::Resovle( IRendererRenderTargets& renderTargets, RenderViewGroup& renderViewGroup )
	{
		TAAResolveProcessor resolveProcessor;

		PrimitiveSubMesh meshInfo;
		meshInfo.m_count = 3;

		auto result = resolveProcessor.Process( meshInfo );
		if ( result.has_value() == false )
		{
			return;
		}

		DrawSnapshot& snapshot = *result;

		agl::Texture* historyTex = renderTargets.GetTAAHistory();
		agl::Texture* resolveTex = renderTargets.GetTAAResolve();
		agl::Texture* sceneTex = renderViewGroup.GetViewport().Texture();
		agl::Texture* velocityTex = renderTargets.GetVelocity();

		agl::ResourceTransition transitions[] = {
			Transition( *historyTex, agl::ResourceState::PixelShaderResource ),
			Transition( *resolveTex, agl::ResourceState::RenderTarget ),
			Transition( *sceneTex, agl::ResourceState::PixelShaderResource ),
			Transition( *velocityTex, agl::ResourceState::PixelShaderResource ),
		};
		GetCommandList().Transition( std::extent_v<decltype( transitions )>, transitions );

		// Linear Sampler
		SamplerOption samplerOption;
		SamplerState historyTexSampler = GraphicsInterface().FindOrCreate( samplerOption );

		// Point Sampler
		samplerOption.m_filter = agl::TextureFilter::Point;
		SamplerState sceneTexSampler = GraphicsInterface().FindOrCreate( samplerOption );

		// Point Sampler
		SamplerState velocityTexSampler = GraphicsInterface().FindOrCreate( samplerOption );

		SceneViewConstantBuffer& viewConstant = renderViewGroup.Scene().SceneViewConstant();

		RenderingShaderResource taaResolveDrawResources;
		taaResolveDrawResources.AddResource( "SceneViewParameters", viewConstant.Resource() );
		taaResolveDrawResources.AddResource( "HistoryTex", historyTex->SRV() );
		taaResolveDrawResources.AddResource( "HistoryTexSampler", historyTexSampler.Resource() );
		taaResolveDrawResources.AddResource( "SceneTex", sceneTex->SRV() );
		taaResolveDrawResources.AddResource( "SceneTexSampler", sceneTexSampler.Resource() );
		taaResolveDrawResources.AddResource( "VelocityTex", velocityTex->SRV() );
		taaResolveDrawResources.AddResource( "VelocityTexSampler", velocityTexSampler.Resource() );
		taaResolveDrawResources.AddResource( "TAAParameter", m_resolveConstantBuffer.Resource() );

		GraphicsPipelineState& pipelineState = snapshot.m_pipelineState;
		taaResolveDrawResources.BindResources( pipelineState.m_shaderState, snapshot.m_shaderBindings );

		agl::RenderTargetView* resolveRt = resolveTex->RTV();

		auto commandList = GetCommandList();
		commandList.BindRenderTargets( &resolveRt, 1, nullptr );

		VisibleDrawSnapshot visibleSnapshot = {
			.m_primitiveId = 0,
			.m_primitiveIdOffset = 0,
			.m_numInstance = 1,
			.m_snapshotBucketId = -1,
			.m_drawSnapshot = &snapshot,
		};

		VertexBuffer emptyPrimitiveID;
		CommitDrawSnapshot( commandList, visibleSnapshot, emptyPrimitiveID );
	}

	void TAARenderer::UpdateHistory( IRendererRenderTargets& renderTargets, RenderViewGroup& renderViewGroup )
	{
		auto commandList = GetCommandList();

		agl::Texture* historyTex = renderTargets.GetTAAHistory();
		agl::Texture* resolveTex = renderTargets.GetTAAResolve();
		agl::Texture* sceneTex = renderViewGroup.GetViewport().Texture();

		agl::ResourceTransition transitions[] = {
			Transition( *historyTex, agl::ResourceState::CopyDest ),
			Transition( *resolveTex, agl::ResourceState::CopySource ),
			Transition( *sceneTex, agl::ResourceState::CopyDest ),
		};

		commandList.Transition( std::extent_v<decltype( transitions )>, transitions );

		commandList.CopyResource( historyTex, resolveTex, true );
		commandList.CopyResource( sceneTex, resolveTex, true );

		agl::Texture* renderTarget = renderViewGroup.GetViewport().Texture();
		agl::RenderTargetView* rtv = renderTarget != nullptr ? renderTarget->RTV() : nullptr;

		agl::Texture* depthStencil = renderTargets.GetDepthStencil();
		agl::DepthStencilView* dsv = depthStencil != nullptr ? depthStencil->DSV() : nullptr;

		agl::ResourceTransition rtTransition = Transition( *sceneTex, agl::ResourceState::RenderTarget );
		commandList.Transition( 1, &rtTransition );

		commandList.BindRenderTargets( &rtv, 1, dsv );
	}

	TAARenderer::TAARenderer()
	{
		m_resolveParam.m_blendWeight = 0.9f;
	}
}