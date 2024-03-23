#include "TemporalAntiAliasingRendering.h"

#include "CommandList.h"
#include "CommonRenderResource.h"
#include "GlobalShaders.h"
#include "GraphicsApiResource.h"
#include "RenderView.h"
#include "ResourceBarrierUtils.h"
#include "Scene/IScene.h"
#include "Scene/PrimitiveSceneInfo.h"
#include "SceneRenderer.h"
#include "ShaderParameterUtils.h"
#include "StaticState.h"
#include "VertexCollection.h"
#include "Viewport.h"

namespace rendercore
{
	class TAAResolvePS final : public GlobalShaderCommon<PixelShader, TAAResolvePS>
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

			GetCommandList().AddTransition( Transition( *historyTex, agl::ResourceState::CopyDest ) );
			GetCommandList().AddTransition( Transition( *sceneTex, agl::ResourceState::CopySource ) );

			GetCommandList().CopyResource( historyTex, sceneTex, true );

			GetCommandList().AddTransition( Transition( *sceneTex, agl::ResourceState::RenderTarget ) );
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
		auto commandList = GetCommandList();

		agl::Texture* historyTex = renderTargets.GetTAAHistory();
		agl::Texture* resolveTex = renderTargets.GetTAAResolve();
		agl::Texture* sceneTex = renderViewGroup.GetViewport().Texture();
		agl::Texture* velocityTex = renderTargets.GetVelocity();

		commandList.AddTransition( Transition( *historyTex, agl::ResourceState::PixelShaderResource ) );
		commandList.AddTransition( Transition( *resolveTex, agl::ResourceState::RenderTarget ) );
		commandList.AddTransition( Transition( *sceneTex, agl::ResourceState::PixelShaderResource ) );
		commandList.AddTransition( Transition( *velocityTex, agl::ResourceState::PixelShaderResource ) );

		commandList.ClearRenderTarget( resolveTex->RTV(), { 0, 0, 0, 0 } );

		TAAResolveProcessor resolveProcessor;
		auto result = resolveProcessor.Process( FullScreenQuadDrawInfo() );
		if ( result.has_value() == false )
		{
			return;
		}

		DrawSnapshot& snapshot = *result;

		// Linear Sampler
		SamplerState historyTexSampler = StaticSamplerState<>::Get();

		// Point Sampler
		SamplerState sceneTexSampler = StaticSamplerState<agl::TextureFilter::Point>::Get();

		// Point Sampler
		SamplerState velocityTexSampler = StaticSamplerState<agl::TextureFilter::Point>::Get();

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

		commandList.BindRenderTargets( &resolveRt, 1, nullptr );

		AddSingleDrawPass( snapshot );
	}

	void TAARenderer::UpdateHistory( IRendererRenderTargets& renderTargets, RenderViewGroup& renderViewGroup )
	{
		auto commandList = GetCommandList();

		agl::Texture* historyTex = renderTargets.GetTAAHistory();
		agl::Texture* resolveTex = renderTargets.GetTAAResolve();
		agl::Texture* sceneTex = renderViewGroup.GetViewport().Texture();

		commandList.AddTransition( Transition( *historyTex, agl::ResourceState::CopyDest ) );
		commandList.AddTransition( Transition( *resolveTex, agl::ResourceState::CopySource ) );
		commandList.AddTransition( Transition( *sceneTex, agl::ResourceState::CopyDest ) );

		commandList.CopyResource( historyTex, resolveTex, true );
		commandList.CopyResource( sceneTex, resolveTex, true );

		agl::Texture* renderTarget = renderViewGroup.GetViewport().Texture();
		agl::RenderTargetView* rtv = renderTarget != nullptr ? renderTarget->RTV() : nullptr;

		agl::Texture* depthStencil = renderTargets.GetDepthStencil();
		agl::DepthStencilView* dsv = depthStencil != nullptr ? depthStencil->DSV() : nullptr;

		commandList.AddTransition( Transition( *sceneTex, agl::ResourceState::RenderTarget ) );

		commandList.BindRenderTargets( &rtv, 1, dsv );
	}

	TAARenderer::TAARenderer()
	{
		m_resolveParam.m_blendWeight = 0.9f;
	}
}