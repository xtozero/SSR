#include "ForwardRenderer.h"

#include "CommandList.h"
#include "CommonRenderResource.h"
#include "Config/DefaultRenderCoreConfig.h"
#include "CpuProfiler.h"
#include "GpuProfiler.h"
#include "GraphicsResourcePool.h"
#include "Math/Vector.h"
#include "Proxies/LightProxy.h"
#include "Proxies/TexturedSkyProxy.h"
#include "RenderGraph.h"
#include "RenderView.h"
#include "ResourceBarrierUtils.h"
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

	agl::Texture* ForwardRendererRenderTargets::GetDepthStencil()
	{
		AllocDepthStencil();
		return m_depthStencil.Get();
	}

	agl::Texture* ForwardRendererRenderTargets::GetViewSpaceDistance()
	{
		AllocViewSpaceDistance();
		return m_linearDepth.Get();
	}

	agl::Texture* ForwardRendererRenderTargets::GetTAAHistory()
	{
		AllocTAARenderTargets();
		return m_taaHistory.Get();
	}

	agl::Texture* ForwardRendererRenderTargets::GetTAAResolve()
	{
		AllocTAARenderTargets();
		return m_taaResolve.Get();
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

	void ForwardRendererRenderTargets::AllocDepthStencil()
	{
		if ( m_depthStencil == nullptr )
		{
			agl::TextureTrait trait = {
				.m_width = m_bufferSize.first,
				.m_height = m_bufferSize.second,
				.m_depth = 1,
				.m_sampleCount = 1,
				.m_sampleQuality = 0,
				.m_mipLevels = 1,
				.m_format = agl::ResourceFormat::D24_UNORM_S8_UINT,
				.m_access = agl::ResourceAccessFlag::Default,
				.m_bindType = agl::ResourceBindType::DepthStencil,
				.m_miscFlag = agl::ResourceMisc::None,
				.m_clearValue = agl::ResourceClearValue{
					.m_depthStencil = {
						.m_depth = 1.f,
						.m_stencil = 0
					}
				}
			};

			m_depthStencil = agl::Texture::Create( trait, "Scene.DepthStencil" );
			EnqueueRenderTask(
				[depthStencil = m_depthStencil]()
				{
					depthStencil->Init();
				} );
		}
	}

	void ForwardRendererRenderTargets::AllocViewSpaceDistance()
	{
		if ( m_linearDepth == nullptr )
		{
			agl::TextureTrait trait = {
				.m_width = m_bufferSize.first,
				.m_height = m_bufferSize.second,
				.m_depth = 1,
				.m_sampleCount = 1,
				.m_sampleQuality = 0,
				.m_mipLevels = 1,
				.m_format = agl::ResourceFormat::R32_FLOAT,
				.m_access = agl::ResourceAccessFlag::Default,
				.m_bindType = agl::ResourceBindType::RenderTarget | agl::ResourceBindType::ShaderResource,
				.m_miscFlag = agl::ResourceMisc::None,
				.m_clearValue = agl::ResourceClearValue{
					.m_color = { 0.f, 0.f, 0.f, 0.f }
				}
			};

			m_linearDepth = GraphicsResourcePool::GetInstance().FindFreeTexture( trait, "Scene.ViewSpaceDistance" );
		}
	}

	void ForwardRendererRenderTargets::AllocTAARenderTargets()
	{
		if ( m_taaHistory == nullptr )
		{
			agl::TextureTrait trait = {
				.m_width = m_bufferSize.first,
				.m_height = m_bufferSize.second,
				.m_depth = 1,
				.m_sampleCount = 1,
				.m_sampleQuality = 0,
				.m_mipLevels = 1,
				.m_format = agl::ResourceFormat::R8G8B8A8_UNORM_SRGB,
				.m_access = agl::ResourceAccessFlag::Default,
				.m_bindType = agl::ResourceBindType::RenderTarget | agl::ResourceBindType::ShaderResource,
				.m_miscFlag = agl::ResourceMisc::None
			};

			m_taaHistory = GraphicsResourcePool::GetInstance().FindFreeTexture( trait, "TAA.History" );
		}

		if ( m_taaResolve == nullptr )
		{
			agl::TextureTrait trait = {
				.m_width = m_bufferSize.first,
				.m_height = m_bufferSize.second,
				.m_depth = 1,
				.m_sampleCount = 1,
				.m_sampleQuality = 0,
				.m_mipLevels = 1,
				.m_format = agl::ResourceFormat::R8G8B8A8_UNORM_SRGB,
				.m_access = agl::ResourceAccessFlag::Default,
				.m_bindType = agl::ResourceBindType::RenderTarget | agl::ResourceBindType::ShaderResource,
				.m_miscFlag = agl::ResourceMisc::None,
				.m_clearValue = agl::ResourceClearValue{
					.m_color = { 0.f, 0.f, 0.f, 0.f }
				}
			};

			m_taaResolve = GraphicsResourcePool::GetInstance().FindFreeTexture( trait, "TAA.Resolve" );
		}
	}

	void ForwardRendererRenderTargets::AllocWorldNormal()
	{
		if ( m_worldNormal == nullptr )
		{
			agl::TextureTrait trait = {
				.m_width = m_bufferSize.first,
				.m_height = m_bufferSize.second,
				.m_depth = 1,
				.m_sampleCount = 1,
				.m_sampleQuality = 0,
				.m_mipLevels = 1,
				.m_format = agl::ResourceFormat::R10G10B10A2_UNORM,
				.m_access = agl::ResourceAccessFlag::Default,
				.m_bindType = agl::ResourceBindType::RenderTarget | agl::ResourceBindType::ShaderResource,
				.m_miscFlag = agl::ResourceMisc::None,
				.m_clearValue = agl::ResourceClearValue{
					.m_color = { 0.f, 0.f, 0.f, 0.f }
				}
			};

			m_worldNormal = GraphicsResourcePool::GetInstance().FindFreeTexture( trait, "Scene.WorldNormal" );
		}
	}

	void ForwardRendererRenderTargets::AllocVelocity()
	{
		if ( m_velocity == nullptr )
		{
			agl::TextureTrait trait = {
				.m_width = m_bufferSize.first,
				.m_height = m_bufferSize.second,
				.m_depth = 1,
				.m_sampleCount = 1,
				.m_sampleQuality = 0,
				.m_mipLevels = 1,
				.m_format = agl::ResourceFormat::R16G16_FLOAT,
				.m_access = agl::ResourceAccessFlag::Default,
				.m_bindType = agl::ResourceBindType::RenderTarget | agl::ResourceBindType::ShaderResource,
				.m_miscFlag = agl::ResourceMisc::None,
				.m_clearValue = agl::ResourceClearValue{
					.m_color = { 0.f, 0.f, 0.f, 0.f }
				}
			};

			m_velocity = GraphicsResourcePool::GetInstance().FindFreeTexture( trait, "Scene.Velocity" );
		}
	}

	void ForwardRendererRenderTargets::ReleaseAll()
	{
		m_depthStencil = nullptr;
		m_linearDepth = nullptr;
		m_taaHistory = nullptr;
		m_taaResolve = nullptr;
		m_worldNormal = nullptr;
		m_velocity = nullptr;
	}

	void ForwardRenderer::PreRender( RenderGraph& renderGraph, RenderViewGroup& renderViewGroup )
	{
		CPU_PROFILE( ForwardRenderer_PreRender );

		SceneRenderer::PreRender( renderGraph, renderViewGroup );

		InitDynamicShadows( renderViewGroup );

		auto rendertargetSize = renderViewGroup.GetViewport().SizeOnRenderThread();
		m_renderTargets.UpdateBufferSize( rendertargetSize.first, rendertargetSize.second );

		IScene& scene = renderViewGroup.Scene();

		m_resourceBinder.Add( "primitiveInfo", scene.GpuPrimitiveInfo().SRV() );
		m_resourceBinder.Add( "ViewSpaceDistance", m_renderTargets.GetViewSpaceDistance()->SRV() );
		m_resourceBinder.Add( "WorldNormal", m_renderTargets.GetWorldNormal()->SRV() );

		SamplerState linearSampler = StaticSamplerState<>::Get();
		m_resourceBinder.Add( "ViewSpaceDistanceSampler", linearSampler.Resource() );
		m_resourceBinder.Add( "WorldNormalSampler", linearSampler.Resource() );

		auto renderScene = scene.GetRenderScene();
		if ( TexturedSkyProxy* proxy = renderScene->TexturedSky() )
		{
			m_resourceBinder.Add( "IrradianceMap", proxy->IrradianceMap()->SRV() );
			m_resourceBinder.Add( "PrefilterMap", proxy->PrefilteredColor()->SRV() );
			m_resourceBinder.Add( "BrdfLUT", BRDFLookUpTexture->SRV() );
		}
		else
		{
			m_resourceBinder.Add( "IrradianceMap", BlackCubeTexture->SRV() );
			m_resourceBinder.Add( "PrefilterMap", BlackCubeTexture->SRV() );
			m_resourceBinder.Add( "BrdfLUT", BlackTexture->SRV() );
		}

		UpdateLightResource( scene );
	}

	void ForwardRenderer::Render( RenderGraph& renderGraph, RenderViewGroup& renderViewGroup )
	{
		CPU_PROFILE( ForwardRenderer_Render );

		m_dynamicVertexBuffer.Commit();

		Viewport& viewport = renderViewGroup.GetViewport();
		viewport.Clear();

		RenderShadowDepthPass( renderGraph );

		IScene& scene = renderViewGroup.Scene();
		if ( Scene* renderScene = scene.GetRenderScene() )
		{
			RenderAtmosphereLookUpTables( renderGraph, *renderScene );
		}

		auto& viewShaderArguments = scene.GetViewShaderArguments();

		for ( uint32 i = 0; i < static_cast<uint32>( renderViewGroup.NumRenderView() ); ++i )
		{
			SceneViewParameters viewParam = GetViewParameters( ( i < m_prevFrameContext.size() ) ? &m_prevFrameContext[i] : nullptr
				, renderViewGroup, i );

			viewShaderArguments.Update( viewParam );

			m_resourceBinder.Add( &viewShaderArguments );

			RenderDepthPass( renderGraph, renderViewGroup, i );
			
			RenderOcclusionTest( renderGraph, renderViewGroup, i );

			RenderIndirectIllumination( renderGraph, renderViewGroup );
			
			RenderDefaultPass( renderGraph, renderViewGroup, i );

			RenderShadow( renderGraph, renderViewGroup );

			RenderSkyAtmosphere( renderGraph, renderViewGroup, i );

			RenderVolumetricCloud( renderGraph, renderViewGroup );

			RenderVolumetricFog( renderGraph, renderViewGroup );
		}

		if ( DefaultRenderCore::IsTaaEnabled() )
		{
			RenderTemporalAntiAliasing( renderGraph, renderViewGroup );
		}
	}

	void ForwardRenderer::RenderHitProxy( RenderGraph& renderGraph, RenderViewGroup& renderViewGroup )
	{
		CPU_PROFILE( ForwardRenderer_RenderHitProxy );

		DoRenderHitProxy( renderGraph, renderViewGroup );
	}

	void ForwardRenderer::RenderDefaultPass( RenderGraph& renderGraph, RenderViewGroup& renderViewGroup, uint32 curView )
	{
		CPU_PROFILE( ForwardRenderer_RenderDefaultPass );

		auto renderTarget = renderViewGroup.GetViewport().Texture();
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

		auto commandList = GetCommandList();
		{
			RenderTexturedSky( renderGraph, scene, rasterOutput );
		}

		{
			GPU_PROFILE_EVENT( renderGraph, Default );
			PIPELINE_STAT_EVENT( renderGraph, Default );

			std::deque<DrawSnapshot> snapshotStorage;
			RenderThreadFrameData<VisibleDrawSnapshot>* pSnapshots = GatherDrawsnapshots( scene, RenderPassType::Default, curView, snapshotStorage );

			if ( pSnapshots != nullptr )
			{
				RenderThreadFrameData<VisibleDrawSnapshot>& snapshots = *pSnapshots;

				VertexBuffer primitiveIds = GetPrimitiveIdPool().Alloc( static_cast<uint32>( snapshots.size() * sizeof( uint32 ) ) );
				SortDrawSnapshots( snapshots, primitiveIds );

				m_resourceBinder.Add( "IndirectIllumination", m_resourceCollection.m_indirectIllumination->SRV() );

				BEGIN_RG_RESOURCE_STRUCT( DefaultPassResource )
					DECLARE_RG_TEXTURE_PIXEL_SRV( indirectIllumination )
				END_RG_RESOURCE_STRUCT();

				auto rgIndirectIllumination = renderGraph.RegisterExternalResource( m_resourceCollection.m_indirectIllumination.Get() );

				DefaultPassResource passResource = {
					.m_indirectIllumination = rgIndirectIllumination
				};

				renderGraph.AddPass(
					passResource,
					rasterOutput,
					[this, &snapshots, storage = std::move( snapshotStorage ), primitiveIds]( CommandList& commandList ) mutable
					{
						// Update invalidated resources
						for ( auto& viewDrawSnapshot : snapshots )
						{
							DrawSnapshot& snapshot = *viewDrawSnapshot.m_drawSnapshot;
							GraphicsPipelineState& pipelineState = snapshot.m_pipelineState;

							m_resourceBinder.Bind( pipelineState.m_shaderState, snapshot.m_shaderBindings );
						}

						ParallelCommitDrawSnapshot( commandList, snapshots, primitiveIds );
					} );
			}
		}

		{
			// TODO
			m_viewInfo[curView].m_debugOverlayData.Draw( renderGraph, m_dynamicVertexBuffer, m_resourceBinder, rasterOutput );
		}
	}

	IRendererRenderTargets& ForwardRenderer::GetRenderRenderTargets()
	{
		return m_renderTargets;
	}

	void ForwardRenderer::RenderDepthPass( RenderGraph& renderGraph, RenderViewGroup& renderViewGroup, uint32 curView )
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
		
		std::deque<DrawSnapshot> snapshotStorage;
		RenderThreadFrameData<VisibleDrawSnapshot>* pSnapshots = GatherDrawsnapshots( scene, RenderPassType::DepthWrite, curView, snapshotStorage );

		if ( pSnapshots != nullptr )
		{
			RenderThreadFrameData<VisibleDrawSnapshot>& snapshots = *pSnapshots;

			VertexBuffer primitiveIds = GetPrimitiveIdPool().Alloc( static_cast<uint32>( snapshots.size() * sizeof( uint32 ) ) );
			SortDrawSnapshots( snapshots, primitiveIds );

			renderGraph.AddPass(
				rasterOutput,
				[this, &snapshots, storage = std::move( snapshotStorage ), primitiveIds]( CommandList& commandList ) mutable
				{
					// Update invalidated resources
					for ( auto& viewDrawSnapshot : snapshots )
					{
						DrawSnapshot& snapshot = *viewDrawSnapshot.m_drawSnapshot;
						GraphicsPipelineState& pipelineState = snapshot.m_pipelineState;

						m_resourceBinder.Bind( pipelineState.m_shaderState, snapshot.m_shaderBindings );
					}

					ParallelCommitDrawSnapshot( commandList, snapshots, primitiveIds );
				} );
		}
		else
		{
			renderGraph.AddPass(
				rasterOutput,
				[]( [[maybe_unused]] ResourceCommandList& commandList )
				{} );
		}
	}

	void ForwardRenderer::RenderOcclusionTest( RenderGraph& renderGraph, RenderViewGroup& renderViewGroup, uint32 viewIndex )
	{
		auto renderTarget = renderViewGroup.GetViewport().Texture();
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
				DoRenderOcclusionTest( commandList, m_resourceBinder, m_viewInfo[viewIndex], m_occlusionRenderData );
			} );
	}

	void ForwardRenderer::UpdateLightResource( IScene& scene )
	{
		Scene* renderScene = scene.GetRenderScene();
		if ( renderScene == nullptr )
		{
			return;
		}

		RenderThreadFrameData<LightSceneInfo*> validLights;
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
				lightParams.ReflectionMipLevels = static_cast<float>( prefilteredColor->GetTrait().m_mipLevels );
			}
		}

		m_forwardLighting.m_shaderArguments->Update( lightParams );

		m_resourceBinder.Add( "ForwardLightConstant", m_forwardLighting.m_shaderArguments->Resource() );
		m_resourceBinder.Add( "ForwardLight", m_forwardLighting.m_lightBuffer.SRV() );
	}
}
