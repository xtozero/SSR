#include "ForwardRenderer.h"

#include "CommandList.h"
#include "CommonRenderResource.h"
#include "CpuProfiler.h"
#include "GpuProfiler.h"
#include "Math/Vector.h"
#include "Proxies/LightProxy.h"
#include "Proxies/TexturedSkyProxy.h"
#include "RenderTargetPool.h"
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
		return m_depthStencil;
	}

	agl::Texture* ForwardRendererRenderTargets::GetViewSpaceDistance()
	{
		AllocViewSpaceDistance();
		return m_linearDepth;
	}

	agl::Texture* ForwardRendererRenderTargets::GetTAAHistory()
	{
		AllocTAARenderTargets();
		return m_taaHistory;
	}

	agl::Texture* ForwardRendererRenderTargets::GetTAAResolve()
	{
		AllocTAARenderTargets();
		return m_taaResolve;
	}

	agl::Texture* ForwardRendererRenderTargets::GetWorldNormal()
	{
		AllocWorldNormal();
		return m_worldNormal;
	}

	agl::Texture* ForwardRendererRenderTargets::GetVelocity()
	{
		AllocVelocity();
		return m_velocity;
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

			m_linearDepth = RenderTargetPool::GetInstance().FindFreeRenderTarget( trait, "Scene.ViewSpaceDistance" );
			EnqueueRenderTask(
				[linearDepth = m_linearDepth]()
				{
					linearDepth->Init();
				} );
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

			m_taaHistory = RenderTargetPool::GetInstance().FindFreeRenderTarget( trait, "TAA.History" );
			EnqueueRenderTask(
				[taaHistory = m_taaHistory]()
				{
					taaHistory->Init();
				} );
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

			m_taaResolve = RenderTargetPool::GetInstance().FindFreeRenderTarget( trait, "TAA.Resolve" );
			EnqueueRenderTask(
				[taaResolve = m_taaResolve]()
				{
					taaResolve->Init();
				} );
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

			m_worldNormal = RenderTargetPool::GetInstance().FindFreeRenderTarget( trait, "Scene.WorldNormal" );
			EnqueueRenderTask(
				[worldNormal = m_worldNormal]()
				{
					worldNormal->Init();
				} );
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

			m_velocity = RenderTargetPool::GetInstance().FindFreeRenderTarget( trait, "Scene.Velocity" );
			EnqueueRenderTask(
				[velocity = m_velocity]()
				{
					velocity->Init();
				} );
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

	bool ForwardRenderer::PreRender( RenderViewGroup& renderViewGroup )
	{
		CPU_PROFILE( ForwardRenderer_PreRender );

		SceneRenderer::PreRender( renderViewGroup );

		auto rendertargetSize = renderViewGroup.GetViewport().SizeOnRenderThread();
		m_renderTargets.UpdateBufferSize( rendertargetSize.first, rendertargetSize.second );

		IScene& scene = renderViewGroup.Scene();
		bool prepared = UpdateGPUPrimitiveInfos( *scene.GetRenderScene() );

		auto& gpuPrimitiveInfo = scene.GpuPrimitiveInfo();
		auto commandList = GetCommandList();

		if ( gpuPrimitiveInfo.Resource() )
		{
			commandList.AddTransition( Transition( *gpuPrimitiveInfo.Resource(), agl::ResourceState::GenericRead ) );
		}

		m_shaderResources.AddResource( "primitiveInfo", gpuPrimitiveInfo.SRV() );
		m_shaderResources.AddResource( "ViewSpaceDistance", m_renderTargets.GetViewSpaceDistance()->SRV() );
		m_shaderResources.AddResource( "WorldNormal", m_renderTargets.GetWorldNormal()->SRV() );

		SamplerState linearSampler = StaticSamplerState<>::Get();
		m_shaderResources.AddResource( "ViewSpaceDistanceSampler", linearSampler.Resource() );
		m_shaderResources.AddResource( "WorldNormalSampler", linearSampler.Resource() );

		auto renderScene = scene.GetRenderScene();
		if ( TexturedSkyProxy* proxy = renderScene->TexturedSky() )
		{
			m_shaderResources.AddResource( "IrradianceMap", proxy->IrradianceMap()->SRV() );
			m_shaderResources.AddResource( "PrefilterMap", proxy->PrefilteredColor()->SRV() );
			m_shaderResources.AddResource( "BrdfLUT", BRDFLookUpTexture->SRV() );
		}
		else
		{
			m_shaderResources.AddResource( "IrradianceMap", BlackCubeTexture->SRV() );
			m_shaderResources.AddResource( "PrefilterMap", BlackCubeTexture->SRV() );
			m_shaderResources.AddResource( "BrdfLUT", BlackTexture->SRV() );
		}

		if ( prepared )
		{
			UpdateLightResource( scene );
		}

		return prepared;
	}

	void ForwardRenderer::Render( RenderViewGroup& renderViewGroup )
	{
		CPU_PROFILE( ForwardRenderer_Render );

		RenderShadowDepthPass();

		IScene& scene = renderViewGroup.Scene();
		if ( Scene* renderScene = scene.GetRenderScene() )
		{
			RenderAtmosphereLookUpTables( *renderScene );
		}

		for ( uint32 i = 0; i < static_cast<uint32>( renderViewGroup.Size() ); ++i )
		{
			auto& viewConstant = scene.SceneViewConstant();

			SceneViewParameters viewConstantParam = FillViewConstantParam( scene.GetRenderScene()
				, ( i < m_prevFrameContext.size() ) ? &m_prevFrameContext[i] : nullptr
				, renderViewGroup, i );

			viewConstant.Update( viewConstantParam );

			auto& view = renderViewGroup[i];

			m_shaderResources.AddResource( "SceneViewParameters", viewConstant.Resource() );

			RenderDepthPass( renderViewGroup, i );
			
			RenderIndirectIllumination( renderViewGroup );
			
			RenderDefaultPass( renderViewGroup, i );

			RenderShadow();

			RenderSkyAtmosphere( scene, view );

			RenderVolumetricCloud( scene, view );

			RenderVolumetricFog( scene, view );
		}
	}

	void ForwardRenderer::PostRender( RenderViewGroup& renderViewGroup )
	{
		CPU_PROFILE( ForwardRenderer_PostRender );

		SceneRenderer::PostRender( renderViewGroup );
	}

	void ForwardRenderer::RenderDefaultPass( RenderViewGroup& renderViewGroup, uint32 curView )
	{
		CPU_PROFILE( ForwardRenderer_RenderDefaultPass );

		auto renderTarget = renderViewGroup.GetViewport().Texture();
		auto depthStencil = m_renderTargets.GetDepthStencil();

		auto [width, height] = renderViewGroup.GetViewport().Size();

		RenderingOutputContext context = {
			.m_renderTargets = { renderTarget },
			.m_depthStencil = depthStencil,
			.m_viewport = { 
				.m_left = 0.f, 
				.m_top = 0.f, 
				.m_front = 0.f,
				.m_right = static_cast<float>( width ), 
				.m_bottom = static_cast<float>( height ), 
				.m_back = 1.f 
			},
			.m_scissorRects = { 
				.m_left = 0L, 
				.m_top = 0L,
				.m_right = static_cast<int32>( width ), 
				.m_bottom = static_cast<int32>( height )
			}
		};
		StoreOuputContext( context );

		IScene& scene = renderViewGroup.Scene();

		auto commandList = GetCommandList();
		{
			GPU_PROFILE( commandList, TexturedSky );
			RenderTexturedSky( scene );
		}

		{
			GPU_PROFILE( commandList, Default );
			RenderMesh( scene, RenderPass::Default, renderViewGroup[curView] );
		}
	}

	IRendererRenderTargets& ForwardRenderer::GetRenderRenderTargets()
	{
		return m_renderTargets;
	}

	void ForwardRenderer::RenderDepthPass( RenderViewGroup& renderViewGroup, uint32 curView )
	{
		CPU_PROFILE( ForwardRenderer_RenderDepthPass );

		auto renderTarget = m_renderTargets.GetViewSpaceDistance();
		auto worldNormal = m_renderTargets.GetWorldNormal();
		auto velocity = m_renderTargets.GetVelocity();
		auto depthStencil = m_renderTargets.GetDepthStencil();

		assert( ( renderTarget != nullptr ) && ( worldNormal != nullptr ) && ( velocity != nullptr ) && ( depthStencil != nullptr ) );

		auto [width, height] = renderViewGroup.GetViewport().Size();

		RenderingOutputContext context = {
			.m_renderTargets = { renderTarget, worldNormal, velocity },
			.m_depthStencil = depthStencil,
			.m_viewport = { 
				.m_left = 0.f, 
				.m_top = 0.f, 
				.m_front = 0.f,
				.m_right = static_cast<float>( width ), 
				.m_bottom = static_cast<float>( height ),
				.m_back = 1.f 
			},
			.m_scissorRects = {
				.m_left = 0L, 
				.m_top = 0L, 
				.m_right = static_cast<int32>( width ),
				.m_bottom = static_cast<int32>( height )
			}
		};
		StoreOuputContext( context );


		auto commandList = GetCommandList();
		{
			GPU_PROFILE( commandList, Depth );

			commandList.AddTransition( Transition( *renderTarget, agl::ResourceState::RenderTarget ) );
			commandList.AddTransition( Transition( *worldNormal, agl::ResourceState::RenderTarget ) );
			commandList.AddTransition( Transition( *velocity, agl::ResourceState::RenderTarget ) );
			commandList.AddTransition( Transition( *depthStencil, agl::ResourceState::DepthWrite ) );

			agl::RenderTargetView* rtv0 = renderTarget->RTV();
			commandList.ClearRenderTarget( rtv0, { 0, 0, 0, 0 } );

			agl::RenderTargetView* rtv1 = worldNormal->RTV();
			commandList.ClearRenderTarget( rtv1, { } );

			agl::RenderTargetView* rtv2 = velocity->RTV();
			commandList.ClearRenderTarget( rtv2, { } );

			agl::DepthStencilView* dsv = depthStencil->DSV();
			commandList.ClearDepthStencil( dsv, 1.f, 0 );

			IScene& scene = renderViewGroup.Scene();

			RenderMesh( scene, RenderPass::DepthWrite, renderViewGroup[curView] );
		}
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

		ForwardLightConstant lightConstant = {
			.m_numLight = static_cast<uint32>( validLights.size() ),
			.m_hemisphereLightUpVector = Vector4::ZeroVector,
			.m_hemisphereLightUpperColor = ColorF::Black,
			.m_hemisphereLightLowerColor = ColorF::Black,
			.m_reflectionMipLevels = 1,
		};

		if ( renderScene->HemisphereLight() )
		{
			const HemisphereLightProxy& hemisphereLight = *renderScene->HemisphereLight();

			lightConstant.m_hemisphereLightUpVector = hemisphereLight.UpVector();
			lightConstant.m_hemisphereLightUpperColor = hemisphereLight.UpperColor();
			lightConstant.m_hemisphereLightLowerColor = hemisphereLight.LowerColor();
		}

		if ( renderScene->TexturedSky() )
		{
			const TexturedSkyProxy& texturedSkyProxy = *renderScene->TexturedSky();
			const auto& irradianceMapSH = texturedSkyProxy.IrradianceMapSH();

			std::memcpy( lightConstant.m_irrdianceMapSH, irradianceMapSH.data(), sizeof( Vector ) * 9 );

			if ( agl::RefHandle<agl::Texture> prefilteredColor = texturedSkyProxy.PrefilteredColor() )
			{
				lightConstant.m_reflectionMipLevels = static_cast<float>( prefilteredColor->GetTrait().m_mipLevels );
			}
		}

		m_forwardLighting.m_lightConstant.Update( lightConstant );

		m_shaderResources.AddResource( "ForwardLightConstant", m_forwardLighting.m_lightConstant.Resource() );
		m_shaderResources.AddResource( "ForwardLight", m_forwardLighting.m_lightBuffer.SRV() );
	}
}
