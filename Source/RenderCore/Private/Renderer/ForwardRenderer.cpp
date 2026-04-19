#include "ForwardRenderer.h"

#include "CommandList.h"
#include "CommonRenderResource.h"
#include "Config/DefaultRenderCoreConfig.h"
#include "CpuProfiler/CpuProfiler.h"
#include "GpuProfiler.h"
#include "GraphicsResourcePool.h"
#include "Math/Vector.h"
#include "Proxies/LightProxy.h"
#include "Proxies/TexturedSkyProxy.h"
#include "RenderGraph.h"
#include "RenderView.h"
#include "RTAORendering.h"
#include "Scene/LightSceneInfo.h"
#include "Scene/Scene.h"
#include "Scene/SceneConstantBuffers.h"
#include "SkyAtmosphereRendering.h"
#include "StaticState.h"
#include "Viewport.h"

namespace rendercore
{
	void ForwardRendererRenderTargets::UpdateBufferSize( uint32 width, uint32 height )
	{
		std::pair<uint32, uint32> newBufferSize( width, height );
		if ( m_bufferSize != newBufferSize )
		{
			ReleaseAll();
			m_bufferSize = newBufferSize;
		}
	}

	void ForwardRendererRenderTargets::Tick()
	{
		std::swap( m_linearDepth[0], m_linearDepth[1] );
	}

	agl::Texture* ForwardRendererRenderTargets::GetSceneColor()
	{
		AllocSceneColor();
		return m_sceneColor.Get();
	}

	agl::Texture* ForwardRendererRenderTargets::GetDepthStencil()
	{
		AllocDepthStencil();
		return m_depthStencil.Get();
	}

	agl::Texture* ForwardRendererRenderTargets::GetViewSpaceDistance()
	{
		AllocViewSpaceDistance();
		return m_linearDepth[0].Get();
	}

	agl::Texture* ForwardRendererRenderTargets::GetPrevViewSpaceDistance()
	{
		AllocViewSpaceDistance();
		return m_linearDepth[1].Get();
	}

	agl::Texture* ForwardRendererRenderTargets::GetWorldNormal()
	{
		AllocWorldNormal();
		return m_worldNormal.Get();
	}

	agl::Texture* ForwardRendererRenderTargets::GetVelocity()
	{
		AllocVelocity();
		return m_velocity.Get();
	}

	agl::Texture* ForwardRendererRenderTargets::GetVisibility()
	{
		AllocVisibility();
		return m_visibility.Get();
	}

	void ForwardRendererRenderTargets::AllocSceneColor()
	{
		if ( m_sceneColor.Get() == nullptr )
		{
			const float4& clearColor = DefaultRenderCore::GetDefaultBackgroundColor();
			agl::TextureDesc desc = {
				.m_width = m_bufferSize.first,
				.m_height = m_bufferSize.second,
				.m_depth = 1,
				.m_sampleCount = 1,
				.m_sampleQuality = 0,
				.m_mipLevels = 1,
				.m_format = agl::ResourceFormat::R8G8B8A8_UNORM,
				.m_access = agl::ResourceAccess::Default,
				.m_bindType = agl::ResourceBindType::ShaderResource | agl::ResourceBindType::RenderTarget | agl::ResourceBindType::RandomAccess,
				.m_miscFlag = agl::ResourceMisc::None,
				.m_clearValue = agl::ResourceClearValue{
					.m_color = { clearColor[0], clearColor[1], clearColor[2], clearColor[3] }
				}
			};

			m_sceneColor = agl::Texture::Create( desc, "Scene.SceneColor" );
		}
	}

	void ForwardRendererRenderTargets::AllocDepthStencil()
	{
		if ( m_depthStencil == nullptr )
		{
			agl::TextureDesc desc = {
				.m_width = m_bufferSize.first,
				.m_height = m_bufferSize.second,
				.m_depth = 1,
				.m_sampleCount = 1,
				.m_sampleQuality = 0,
				.m_mipLevels = 1,
				.m_format = agl::ResourceFormat::D24_UNORM_S8_UINT,
				.m_access = agl::ResourceAccess::Default,
				.m_bindType = agl::ResourceBindType::DepthStencil,
				.m_miscFlag = agl::ResourceMisc::None,
				.m_clearValue = agl::ResourceClearValue{
					.m_depthStencil = {
						.m_depth = 1.f,
						.m_stencil = 0
					}
				}
			};

			m_depthStencil = agl::Texture::Create( desc, "Scene.DepthStencil" );
		}
	}

	void ForwardRendererRenderTargets::AllocViewSpaceDistance()
	{
		if ( m_linearDepth[0] == nullptr || m_linearDepth[1] == nullptr )
		{
			agl::TextureDesc desc = {
				.m_width = m_bufferSize.first,
				.m_height = m_bufferSize.second,
				.m_depth = 1,
				.m_sampleCount = 1,
				.m_sampleQuality = 0,
				.m_mipLevels = 1,
				.m_format = agl::ResourceFormat::R32_FLOAT,
				.m_access = agl::ResourceAccess::Default,
				.m_bindType = agl::ResourceBindType::RenderTarget | agl::ResourceBindType::ShaderResource,
				.m_miscFlag = agl::ResourceMisc::None,
				.m_clearValue = agl::ResourceClearValue{
					.m_color = { 0.f, 0.f, 0.f, 0.f }
				}
			};

			m_linearDepth[0] = GraphicsResourcePool::GetInstance().FindFreeTexture( desc, "Scene.ViewSpaceDistance" );
			m_linearDepth[1] = GraphicsResourcePool::GetInstance().FindFreeTexture( desc, "Scene.PrevViewSpaceDistance" );
		}
	}

	void ForwardRendererRenderTargets::AllocWorldNormal()
	{
		if ( m_worldNormal == nullptr )
		{
			agl::TextureDesc desc = {
				.m_width = m_bufferSize.first,
				.m_height = m_bufferSize.second,
				.m_depth = 1,
				.m_sampleCount = 1,
				.m_sampleQuality = 0,
				.m_mipLevels = 1,
				.m_format = agl::ResourceFormat::R10G10B10A2_UNORM,
				.m_access = agl::ResourceAccess::Default,
				.m_bindType = agl::ResourceBindType::RenderTarget | agl::ResourceBindType::ShaderResource,
				.m_miscFlag = agl::ResourceMisc::None,
				.m_clearValue = agl::ResourceClearValue{
					.m_color = { 0.f, 0.f, 0.f, 0.f }
				}
			};

			m_worldNormal = GraphicsResourcePool::GetInstance().FindFreeTexture( desc, "Scene.WorldNormal" );
		}
	}

	void ForwardRendererRenderTargets::AllocVelocity()
	{
		if ( m_velocity == nullptr )
		{
			agl::TextureDesc desc = {
				.m_width = m_bufferSize.first,
				.m_height = m_bufferSize.second,
				.m_depth = 1,
				.m_sampleCount = 1,
				.m_sampleQuality = 0,
				.m_mipLevels = 1,
				.m_format = agl::ResourceFormat::R16G16_FLOAT,
				.m_access = agl::ResourceAccess::Default,
				.m_bindType = agl::ResourceBindType::RenderTarget | agl::ResourceBindType::ShaderResource,
				.m_miscFlag = agl::ResourceMisc::None,
				.m_clearValue = agl::ResourceClearValue{
					.m_color = { 0.f, 0.f, 0.f, 0.f }
				}
			};

			m_velocity = GraphicsResourcePool::GetInstance().FindFreeTexture( desc, "Scene.Velocity" );
		}
	}

	void ForwardRendererRenderTargets::AllocVisibility()
	{
		if ( m_visibility == nullptr )
		{
			agl::TextureDesc desc = {
				.m_width = m_bufferSize.first,
				.m_height = m_bufferSize.second,
				.m_depth = 1,
				.m_sampleCount = 1,
				.m_sampleQuality = 0,
				.m_mipLevels = 1,
				.m_format = agl::ResourceFormat::R32_UINT,
				.m_access = agl::ResourceAccess::Default,
				.m_bindType = agl::ResourceBindType::RenderTarget | agl::ResourceBindType::ShaderResource,
				.m_miscFlag = agl::ResourceMisc::None,
				.m_clearValue = agl::ResourceClearValue{
					.m_color = { 0.f, 0.f, 0.f, 0.f }
				}
			};

			m_visibility = GraphicsResourcePool::GetInstance().FindFreeTexture( desc, "Scene.Visibility" );
		}
	}

	void ForwardRendererRenderTargets::ReleaseAll()
	{
		m_sceneColor = nullptr;
		m_depthStencil = nullptr;
		m_linearDepth[0] = nullptr;
		m_linearDepth[1] = nullptr;
		m_worldNormal = nullptr;
		m_velocity = nullptr;
	}

	void ForwardRenderer::PreRender( RenderGraph& renderGraph, RenderViewGroup& renderViewGroup )
	{
		CPU_PROFILE( ForwardRenderer_PreRender );

		SceneRenderer::PreRender( renderGraph, renderViewGroup );

		InitDynamicShadows( renderViewGroup );

		m_renderTargets.Tick();

		auto rendertargetSize = renderViewGroup.GetViewport().SizeOnRenderThread();
		m_renderTargets.UpdateBufferSize( rendertargetSize.first, rendertargetSize.second );

		IScene& scene = renderViewGroup.Scene();

		m_resourceBinder.Add( StaticName( "primitiveInfo" ), scene.GpuPrimitiveInfo().SRV() );
		m_resourceBinder.Add( StaticName( "ViewSpaceDistance" ), m_renderTargets.GetViewSpaceDistance()->SRV() );
		m_resourceBinder.Add( StaticName( "WorldNormal" ), m_renderTargets.GetWorldNormal()->SRV() );

		SamplerState linearSampler = StaticSamplerState<>::Get();
		m_resourceBinder.Add( StaticName( "ViewSpaceDistanceSampler" ), linearSampler.Resource() );
		m_resourceBinder.Add( StaticName( "WorldNormalSampler" ), linearSampler.Resource() );

		auto renderScene = scene.GetRenderScene();
		if ( TexturedSkyProxy* proxy = renderScene->TexturedSky() )
		{
			m_resourceBinder.Add( StaticName( "IrradianceMap" ), proxy->IrradianceMap()->SRV() );
			m_resourceBinder.Add( StaticName( "PrefilterMap" ), proxy->PrefilteredColor()->SRV() );
			m_resourceBinder.Add( StaticName( "BrdfLUT" ), BRDFLookUpTexture->SRV() );
		}
		else
		{
			m_resourceBinder.Add( StaticName( "IrradianceMap" ), BlackCubeTexture->SRV() );
			m_resourceBinder.Add( StaticName( "PrefilterMap" ), BlackCubeTexture->SRV() );
			m_resourceBinder.Add( StaticName( "BrdfLUT" ), BlackTexture->SRV() );
		}

		UpdateLightResource( scene );
	}

	void ForwardRenderer::Render( RenderGraph& renderGraph, RenderViewGroup& renderViewGroup )
	{
		CPU_PROFILE( ForwardRenderer_Render );

		m_dynamicVertexBuffer.Commit();

		Viewport& viewport = renderViewGroup.GetViewport();
		viewport.Clear();

		ClearRenderTargets( renderGraph );

		RenderShadowDepthPass( renderGraph );

		IScene& scene = renderViewGroup.Scene();
		if ( Scene* renderScene = scene.GetRenderScene() )
		{
			RenderAtmosphereLookUpTables( renderGraph, *renderScene );
		}

		auto& viewShaderArguments = scene.GetViewShaderArguments();

		for ( uint32 i = 0; i < static_cast<uint32>( renderViewGroup.NumRenderView() ); ++i )
		{
			SceneViewParameters viewParam = GetViewParameters( ( i < m_prevFrameContext.size() ) ? &m_prevFrameContext[i] : nullptr, renderViewGroup, i );

			viewShaderArguments.Update( viewParam );

			m_resourceBinder.Add( &viewShaderArguments );

			RenderDepthPass( renderGraph, renderViewGroup, i );
			
			RenderOcclusionTest( renderGraph, renderViewGroup, i );

			RenderAmbientOcclusion( renderGraph, renderViewGroup );

			RenderIndirectIllumination( renderGraph, renderViewGroup, i );
			
			RenderDefaultPass( renderGraph, renderViewGroup, i );

			RenderScreenSpaceIndirectIllumination( renderGraph, renderViewGroup, i );

			RenderShadow( renderGraph, renderViewGroup );

			RenderSkyAtmosphere( renderGraph, renderViewGroup, i );

			RenderVolumetricCloud( renderGraph, renderViewGroup );

			RenderVolumetricFog( renderGraph, renderViewGroup );

			RenderEditorOutline( renderGraph, renderViewGroup );
		}

		if ( DefaultRenderCore::IsTaaEnabled() )
		{
			RenderTemporalAntiAliasing( renderGraph, renderViewGroup );
		}

		ResolveSceneColor( renderGraph, renderViewGroup );
	}

	void ForwardRenderer::RenderHitProxy( RenderGraph& renderGraph, RenderViewGroup& renderViewGroup )
	{
		CPU_PROFILE( ForwardRenderer_RenderHitProxy );

		DoRenderHitProxy( renderGraph, renderViewGroup );
	}

	void ForwardRenderer::RenderDefaultPass( RenderGraph& renderGraph, RenderViewGroup& renderViewGroup, uint32 viewIndex )
	{
		CPU_PROFILE( ForwardRenderer_RenderDefaultPass );

		auto renderTarget = m_renderTargets.GetSceneColor();
		auto depthStencil = m_renderTargets.GetDepthStencil();

		auto rgRenderTarget = renderGraph.RegisterExternalResource( renderTarget );
		auto rgDepthStencil = renderGraph.RegisterExternalResource( depthStencil );

		auto [width, height] = renderViewGroup.GetViewport().Size();

		RasterOutput rasterOutput;
		rasterOutput.SetRenderTarget( 0, rgRenderTarget );
		rasterOutput.SetDepthStencil( rgDepthStencil, true );
		rasterOutput.SetViewport( width, height );
		rasterOutput.SetScissorRect( width, height );

		IScene& scene = renderViewGroup.Scene();

		RenderTexturedSky( renderGraph, scene, rasterOutput );

		GPU_PROFILE_EVENT( renderGraph, Default );
		PIPELINE_STAT_EVENT( renderGraph, Default );

		m_resourceBinder.Add( StaticName( "AmbientOcclusion" ), m_resourceCollection.m_ambientOcclusion->SRV() );
		m_resourceBinder.Add( StaticName( "IndirectIllumination" ), m_resourceCollection.m_indirectIllumination->SRV() );

		if ( DefaultRenderCore::SupportsVisibilityRendering() )
		{
			RenderDefaultPassWithVisibilityBuffer( renderGraph, renderViewGroup, viewIndex );
		}

		if ( RenderFrameArray<VisibleDrawSnapshot>* passSnapShots = GatherSortedDrawSnapshots( scene, RenderPassType::Default, viewIndex ) )
		{
			GPU_PROFILE_EVENT( renderGraph, NonVisibility )

			RenderFrameArray<VisibleDrawSnapshot>& snapshots = *passSnapShots;

			VertexBuffer primitiveIds = GetPrimitiveIdPool().Alloc( static_cast<uint32>( snapshots.size() * sizeof( uint32 ) ) );
			UpdatePrimitiveIDs( snapshots, primitiveIds );

			BEGIN_RG_RESOURCE_STRUCT( DefaultPassResource )
				DECLARE_RG_TEXTURE_PIXEL_SRV( ambientOcclusion )
				DECLARE_RG_TEXTURE_PIXEL_SRV( indirectIllumination )
			END_RG_RESOURCE_STRUCT();

			auto rgAmbientOcclusion = renderGraph.RegisterExternalResource( m_resourceCollection.m_ambientOcclusion.Get() );
			auto rgIndirectIllumination = renderGraph.RegisterExternalResource( m_resourceCollection.m_indirectIllumination.Get() );

			DefaultPassResource passResource = {
				.m_ambientOcclusion = rgAmbientOcclusion,
				.m_indirectIllumination = rgIndirectIllumination
			};

			renderGraph.AddPass(
				passResource,
				rasterOutput,
				[this, &snapshots, primitiveIds]( CommandList& commandList ) mutable
				{
					CPU_PROFILE( RenderGraph_RenderDefaultPass );

					// Update invalidated resources
					for ( size_t i = 0; i < snapshots.size(); )
					{
						DrawSnapshot& snapshot = *snapshots[i].m_drawSnapshot;
						GraphicsPipelineState& pipelineState = snapshot.m_pipelineState;

						m_resourceBinder.Bind( pipelineState.m_shaderState, snapshot.m_shaderBindings );

						i += snapshots[i].m_numInstance;
					}

					ParallelCommitDrawSnapshot( commandList, snapshots, primitiveIds );
				} );
		}

		RenderDebugOverlay( renderGraph, renderViewGroup, viewIndex );
	}

	IRendererRenderTargets& ForwardRenderer::GetRenderTargets()
	{
		return m_renderTargets;
	}

	void ForwardRenderer::RenderScreenSpaceIndirectIllumination( RenderGraph& renderGraph, RenderViewGroup& renderViewGroup, uint32 viewIndex )
	{
		if ( DefaultRenderCore::IsSSGIEnabled() == false )
		{
			return;
		}

		SceneRenderer::RenderScreenSpaceIndirectIllumination( renderGraph, renderViewGroup, viewIndex );

		GPU_PROFILE_EVENT( renderGraph, CompositeSSGI );
		PIPELINE_STAT_EVENT( renderGraph, CompositeSSGI );

		auto renderTarget = m_renderTargets.GetSceneColor();
		auto depthStencil = m_renderTargets.GetDepthStencil();

		auto rgRenderTarget = renderGraph.RegisterExternalResource( renderTarget );
		auto rgDepthStencil = renderGraph.RegisterExternalResource( depthStencil );

		auto [width, height] = renderViewGroup.GetViewport().Size();

		RasterOutput rasterOutput;
		rasterOutput.SetRenderTarget( 0, rgRenderTarget );
		rasterOutput.SetDepthStencil( rgDepthStencil, true );
		rasterOutput.SetViewport( width, height );
		rasterOutput.SetScissorRect( width, height );

		IScene& scene = renderViewGroup.Scene();
		RenderFrameArray<VisibleDrawSnapshot>* pSnapshots = GatherSortedDrawSnapshots( scene, RenderPassType::CompositeSSGI, viewIndex );

		if ( pSnapshots != nullptr )
		{
			RenderFrameArray<VisibleDrawSnapshot>& snapshots = *pSnapshots;

			VertexBuffer primitiveIds = GetPrimitiveIdPool().Alloc( static_cast<uint32>( snapshots.size() * sizeof( uint32 ) ) );
			UpdatePrimitiveIDs( snapshots, primitiveIds );

			m_resourceBinder.Add( StaticName( "SSGITex" ), m_resourceCollection.m_ssgi->SRV() );

			SamplerState linearSampler = StaticSamplerState<>::Get();
			m_resourceBinder.Add( StaticName( "SSGITexSampler" ), linearSampler.Resource() );

			BEGIN_RG_RESOURCE_STRUCT( CompositeSSGIPassResource )
				DECLARE_RG_TEXTURE_PIXEL_SRV( ssgi )
			END_RG_RESOURCE_STRUCT();

			auto rgSSGI = renderGraph.RegisterExternalResource( m_resourceCollection.m_ssgi.Get() );

			CompositeSSGIPassResource passResource = {
				.m_ssgi = rgSSGI
			};

			renderGraph.AddPass(
				passResource,
				rasterOutput,
				[this, &snapshots, primitiveIds]( CommandList& commandList ) mutable
				{
					// Update invalidated resources
					for ( size_t i = 0; i < snapshots.size(); )
					{
						DrawSnapshot& snapshot = *snapshots[i].m_drawSnapshot;
						GraphicsPipelineState& pipelineState = snapshot.m_pipelineState;

						m_resourceBinder.Bind( pipelineState.m_shaderState, snapshot.m_shaderBindings );

						i += snapshots[i].m_numInstance;
					}

					ParallelCommitDrawSnapshot( commandList, snapshots, primitiveIds );
				} );
		}
	}

	void ForwardRenderer::RenderDepthPass( RenderGraph& renderGraph, RenderViewGroup& renderViewGroup, uint32 viewIndex )
	{
		CPU_PROFILE( ForwardRenderer_RenderDepthPass );

		auto renderTarget = m_renderTargets.GetViewSpaceDistance();
		auto worldNormal = m_renderTargets.GetWorldNormal();
		auto velocity = m_renderTargets.GetVelocity();
		auto depthStencil = m_renderTargets.GetDepthStencil();

		auto rgRenderTarget = renderGraph.RegisterExternalResource( renderTarget );
		auto rgWorldNormal = renderGraph.RegisterExternalResource( worldNormal );
		auto rgVelocity = renderGraph.RegisterExternalResource( velocity );
		auto rgDepthStencil = renderGraph.RegisterExternalResource( depthStencil );

		assert( ( renderTarget != nullptr ) && ( worldNormal != nullptr ) && ( velocity != nullptr ) && ( depthStencil != nullptr ) );

		auto [width, height] = renderViewGroup.GetViewport().Size();

		RasterOutput rasterOutput;
		rasterOutput.SetRenderTarget( 0, rgRenderTarget, RasterOutputLoadAction::Clear );
		rasterOutput.SetRenderTarget( 1, rgWorldNormal, RasterOutputLoadAction::Clear );
		rasterOutput.SetRenderTarget( 2, rgVelocity, RasterOutputLoadAction::Clear );
		rasterOutput.SetDepthStencil( rgDepthStencil, false, RasterOutputLoadAction::Clear );
		rasterOutput.SetViewport( width, height );
		rasterOutput.SetScissorRect( width, height );

		GPU_PROFILE_EVENT( renderGraph, Depth );

		IScene& scene = renderViewGroup.Scene();

		RenderFrameArray<VisibleDrawSnapshot>* pSnapshots = GatherSortedDrawSnapshots( scene, RenderPassType::DepthWrite, viewIndex );

		if ( pSnapshots != nullptr )
		{
			RenderFrameArray<VisibleDrawSnapshot>& snapshots = *pSnapshots;

			VertexBuffer primitiveIds = GetPrimitiveIdPool().Alloc( static_cast<uint32>( snapshots.size() * sizeof( uint32 ) ) );
			UpdatePrimitiveIDs( snapshots, primitiveIds );

			renderGraph.AddPass(
				rasterOutput,
				[this, &snapshots, primitiveIds]( CommandList& commandList ) mutable
				{
					CPU_PROFILE( RenderGraph_RenderDepthPass );

					// Update invalidated resources
					for ( size_t i = 0; i < snapshots.size(); )
					{
						DrawSnapshot& snapshot = *snapshots[i].m_drawSnapshot;
						GraphicsPipelineState& pipelineState = snapshot.m_pipelineState;

						m_resourceBinder.Bind( pipelineState.m_shaderState, snapshot.m_shaderBindings );

						i += snapshots[i].m_numInstance;
					}

					ParallelCommitDrawSnapshot( commandList, snapshots, primitiveIds );
				} );
		}
		else
		{
			renderGraph.AddPass(
				rasterOutput,
				[]( [[maybe_unused]] CommandList& commandList )
				{} );
		}
	}

	void ForwardRenderer::RenderOcclusionTest( RenderGraph& renderGraph, RenderViewGroup& renderViewGroup, uint32 viewIndex )
	{
		auto renderTarget = m_renderTargets.GetSceneColor();
		auto depthStencil = m_renderTargets.GetDepthStencil();

		auto rgRenderTarget = renderGraph.RegisterExternalResource( renderTarget );
		auto rgDepthStencil = renderGraph.RegisterExternalResource( depthStencil );

		auto [width, height] = renderViewGroup.GetViewport().Size();

		RasterOutput rasterOutput;
		rasterOutput.SetRenderTarget( 0, rgRenderTarget );
		rasterOutput.SetDepthStencil( rgDepthStencil, true );
		rasterOutput.SetViewport( width, height );
		rasterOutput.SetScissorRect( width, height );

		GPU_PROFILE_EVENT( renderGraph, Occlusion );

		renderGraph.AddPass(
			rasterOutput,
			[this, viewIndex]( CommandList& commandList )
			{
				CPU_PROFILE( RenderGraph_RenderOcclusionTest );

				DoRenderOcclusionTest( commandList, m_resourceBinder, m_viewInfo[viewIndex], m_occlusionRenderData );
			} );
	}

	void ForwardRenderer::RenderDefaultPassWithVisibilityBuffer( RenderGraph& renderGraph, RenderViewGroup& renderViewGroup, uint32 viewIndex )
	{
		GPU_PROFILE_EVENT( renderGraph, Visibility )

		IScene& scene = renderViewGroup.Scene();
		VisibilityPassData visibilityPassData = BuildVisibilityPassData( scene, viewIndex );
		if ( visibilityPassData.m_shadingSnapshots.empty() )
		{
			return;
		}

		auto rgVisibility = renderGraph.RegisterExternalResource( m_renderTargets.GetVisibility() );

		VisibilityBuffer::RenderBufferParam renderBufferParam = {
			.m_renderViewGroup = renderViewGroup,
			.m_resourceBinder = m_resourceBinder,
			.m_visibilityPassData = visibilityPassData,
			.m_visibility = rgVisibility,
			.m_depthStencil = m_renderTargets.GetDepthStencil(),
		};

		VisibilityBuffer::RenderBuffer( renderGraph, renderBufferParam );

		VisibilityBuffer::CountDrawCallIdParam countDrawCallIDParam = {
			.m_numDrawCallIds = visibilityPassData.m_maxShadingSnapshotId,
			.m_visibility = rgVisibility,
		};

		auto countDrawCallIdOutput = VisibilityBuffer::CountDrawCallId( renderGraph, countDrawCallIDParam );

		VisibilityBuffer::CalcConterPrefixSumParam calcConterPrefixSumParam = {
			.m_numDrawCallIds = visibilityPassData.m_maxShadingSnapshotId,
			.m_counter = countDrawCallIdOutput.m_counter,
		};

		RenderGraphBuffer* rgOffset = VisibilityBuffer::CalcCounterPrefixSum( renderGraph, calcConterPrefixSumParam );

		VisibilityBuffer::BuildWorkListParam buildWorkListParam = {
			.m_numDrawCallIds = visibilityPassData.m_maxShadingSnapshotId,
			.m_visibility = rgVisibility,
			.m_offset = rgOffset,
			.m_indirectArgs = countDrawCallIdOutput.m_indirectArgs,
		};

		RenderGraphBuffer* rgWorkList = VisibilityBuffer::BuildWorkList( renderGraph, buildWorkListParam );

		RefHandle<agl::Buffer> primitiveIds = VisibilityBuffer::UploadPrimitiveIds( visibilityPassData );

		BEGIN_RG_RESOURCE_STRUCT( DefaultPassResource )
			DECLARE_RG_BUFFER_INDRIECT_ARG( indirectArgs )
			DECLARE_RG_TEXTURE_NONPIXEL_SRV( ambientOcclusion )
			DECLARE_RG_TEXTURE_NONPIXEL_SRV( indirectIllumination )
			DECLARE_RG_TEXTURE_NONPIXEL_SRV( visibility )
			DECLARE_RG_BUFFER_NONPIXEL_SRV( counter )
			DECLARE_RG_BUFFER_NONPIXEL_SRV( offset )
			DECLARE_RG_BUFFER_NONPIXEL_SRV( workList )
			DECLARE_RG_BUFFER_NONPIXEL_SRV( primitiveIds )
			DECLARE_RG_TEXTURE_UAV( sceneColor )
		END_RG_RESOURCE_STRUCT();

		auto rgAmbientOcclusion = renderGraph.RegisterExternalResource( m_resourceCollection.m_ambientOcclusion.Get() );
		auto rgIndirectIllumination = renderGraph.RegisterExternalResource( m_resourceCollection.m_indirectIllumination.Get() );
		auto rgPrimitiveIds = renderGraph.RegisterExternalResource( primitiveIds.Get() );
		auto rgSceneColor = renderGraph.RegisterExternalResource( m_renderTargets.GetSceneColor() );

		DefaultPassResource passResource = {
			.m_indirectArgs = countDrawCallIdOutput.m_indirectArgs,
			.m_ambientOcclusion = rgAmbientOcclusion,
			.m_indirectIllumination = rgIndirectIllumination,
			.m_visibility = rgVisibility,
			.m_counter = countDrawCallIdOutput.m_counter,
			.m_offset = rgOffset,
			.m_workList = rgWorkList,
			.m_primitiveIds = rgPrimitiveIds,
			.m_sceneColor = rgSceneColor,
		};

		{
			GPU_PROFILE_EVENT( renderGraph, Shading );

			renderGraph.AddPass(
			passResource,
			[this, passResource, visibilityPassData]( ComputeCommandList& commandList )
			{
				CPU_PROFILE( RenderGraph_VisibilityShading );

				m_resourceBinder.Add( StaticName( "Visibility" ), passResource.m_visibility->SRV() );
				m_resourceBinder.Add( StaticName( "Counter" ), passResource.m_counter->SRV() );
				m_resourceBinder.Add( StaticName( "Offset" ), passResource.m_offset->SRV() );
				m_resourceBinder.Add( StaticName( "WorkList" ), passResource.m_workList->SRV() );
				m_resourceBinder.Add( StaticName( "SceneColor" ), passResource.m_sceneColor->SRV() );
				m_resourceBinder.Add( StaticName( "PrimitiveIds" ), passResource.m_primitiveIds->SRV() );

				uint32 width = passResource.m_visibility->GetDesc().m_width;
				uint32 height = passResource.m_visibility->GetDesc().m_height;

				const RenderFrameArray<VisibleShadingSnapshot>& shadingSnapshots = visibilityPassData.m_shadingSnapshots;
				for ( size_t i = 0; i < shadingSnapshots.size(); ++i )
				{
					const ShadingSnapshot& shadingSnapshot = *shadingSnapshots[i].m_shadingSnapshot;
					commandList.BindPipelineState( shadingSnapshot.m_pso.Get() );

					agl::ShaderBindings shaderBindings = shadingSnapshot.m_shaderBindings;
					m_resourceBinder.Bind( shadingSnapshot.m_computeShader, shaderBindings );

					agl::ShaderParameter drawCallIdParam = shadingSnapshot.m_computeShader->ParameterMap().GetParameter( StaticName( "DrawCallId" ) );
					agl::ShaderParameter screenSizeParam = shadingSnapshot.m_computeShader->ParameterMap().GetParameter( StaticName( "ScreenSize" ) );
					agl::ShaderParameter baseIndexParam = shadingSnapshot.m_computeShader->ParameterMap().GetParameter( StaticName( "BaseIndex" ) );
					agl::ShaderParameter baseVertexParam = shadingSnapshot.m_computeShader->ParameterMap().GetParameter( StaticName( "BaseVertex" ) );

					auto drawCallId = static_cast<uint32>( i + 1 );
					SetShaderValue( commandList, drawCallIdParam, drawCallId );

					uint32 screenSize[2] = {
						width,
						height
					};
					SetShaderValue( commandList, screenSizeParam, screenSize );
					SetShaderValue( commandList, baseIndexParam, shadingSnapshot.m_startIndexLocation );
					SetShaderValue( commandList, baseVertexParam, shadingSnapshot.m_baseVertexLocation );

					commandList.BindShaderResources( shaderBindings );
					commandList.ExecuteIndirect( agl::IndirectCommandType::Dispatch, passResource.m_indirectArgs->Get(), drawCallId );
				}
			} );
		}
	}

	void ForwardRenderer::RenderAmbientOcclusion( RenderGraph& renderGraph, RenderViewGroup& renderViewGroup )
	{
		IScene& scene = renderViewGroup.Scene();

		RaytracingScene* raytracingScene = scene.GetRaytracingScene();
		if ( raytracingScene == nullptr )
		{
			return;
		}

		auto [width, height] = renderViewGroup.GetViewport().Size();

		RTAORenderParams params = {
			.m_viewShaderArguments = scene.GetViewShaderArguments().Resource(),
			.m_raytracingScene = raytracingScene->GetTLAS(),
			.m_prevViewSpaceDistance = GetRenderTargets().GetPrevViewSpaceDistance(),
			.m_viewSpaceDistance = GetRenderTargets().GetViewSpaceDistance(),
			.m_worldNormal = GetRenderTargets().GetWorldNormal(),
			.m_velocity = GetRenderTargets().GetVelocity(),
			.m_screenWidth = width,
			.m_screenHeight = height,
		};

		m_resourceCollection.m_ambientOcclusion = m_rtaoPass.Render( renderGraph, params );
	}

	void ForwardRenderer::UpdateLightResource( IScene& scene )
	{
		Scene* renderScene = scene.GetRenderScene();
		if ( renderScene == nullptr )
		{
			return;
		}

		RenderFrameArray<LightSceneInfo*> validLights;
		const SparseArray<LightSceneInfo*>& lights = renderScene->Lights();
		for ( auto light : lights )
		{
			validLights.emplace_back( light );
		}

		ForwardLightBuffer& lightBuffer = m_forwardLighting.m_lightBuffer;

		uint32 numElement = static_cast<uint32>( ( sizeof( ForwardLightData ) / sizeof( Vector4 ) ) * validLights.size() );
		numElement = std::max<uint32>( 1, numElement );

		lightBuffer.Initialize( sizeof( Vector4 ), numElement, agl::ResourceFormat::R32G32B32A32_FLOAT );

		auto lightData = static_cast<ForwardLightData*>( lightBuffer.Lock() );
		assert( lightData != nullptr );

		uint32 idOnGpu = 0;
		for ( auto light : validLights )
		{
			light->SetIdOnGPU( idOnGpu++ );

			LightProxy* proxy = light->Proxy();
			LightProperty property = proxy->GetLightProperty();

			lightData->m_positionAndRange = Vector4( property.m_position[0], property.m_position[1], property.m_position[2], property.m_range );
			lightData->m_diffuse = property.m_diffuse;
			lightData->m_specular = property.m_specular;
			lightData->m_attenuationAndFalloff = Vector4( property.m_attenuation[0], property.m_attenuation[1], property.m_attenuation[2], property.m_fallOff );
			lightData->m_directionAndType = Vector4( property.m_direction[0], property.m_direction[1], property.m_direction[2], static_cast<float>( property.m_type ) );
			lightData->m_spotAngles = Vector4( property.m_theta, property.m_phi, 0.f, 0.f );

			++lightData;
		}

		lightBuffer.Unlock();

		ForwardLightParameters lightParams = {
			.NumLight = static_cast<uint32>( validLights.size() ),
			.HemisphereLightUpVector = Vector4::ZeroVector,
			.HemisphereLightUpperColor = ColorF::Black,
			.HemisphereLightLowerColor = ColorF::Black,
			.ReflectionMipLevels = 1,
		};

		if ( renderScene->HemisphereLight() )
		{
			const HemisphereLightProxy& hemisphereLight = *renderScene->HemisphereLight();

			lightParams.HemisphereLightUpVector = hemisphereLight.UpVector();
			lightParams.HemisphereLightUpperColor = hemisphereLight.UpperColor();
			lightParams.HemisphereLightLowerColor = hemisphereLight.LowerColor();
		}

		if ( renderScene->TexturedSky() )
		{
			const TexturedSkyProxy& texturedSkyProxy = *renderScene->TexturedSky();
			const auto& irradianceMapSH = texturedSkyProxy.IrradianceMapSH();

			std::memcpy( lightParams.IrradianceMapSH, irradianceMapSH.data(), sizeof( Vector ) * 9 );

			RefHandle<agl::Texture> prefilteredColor = texturedSkyProxy.PrefilteredColor();
			if ( prefilteredColor.Get() )
			{
				lightParams.ReflectionMipLevels = static_cast<float>( prefilteredColor->GetDesc().m_mipLevels );
			}
		}

		m_forwardLighting.m_shaderArguments->Update( lightParams );

		m_resourceBinder.Add( StaticName( "ForwardLightConstant" ), m_forwardLighting.m_shaderArguments->Resource() );
		m_resourceBinder.Add( StaticName( "ForwardLight" ), m_forwardLighting.m_lightBuffer.SRV() );
	}

	void ForwardRenderer::ClearRenderTargets( RenderGraph& renderGraph )
	{
		auto rgSceneColor = renderGraph.RegisterExternalResource( m_renderTargets.GetSceneColor() );

		RasterOutput rasterOutput;
		rasterOutput.SetRenderTarget( 0, rgSceneColor, RasterOutputLoadAction::Clear );

		renderGraph.AddPass(
			rasterOutput,
			[]( [[maybe_unused]] CommandList& commandList )
			{
			} );
	}
}
