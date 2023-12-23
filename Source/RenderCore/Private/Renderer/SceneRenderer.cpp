#include "stdafx.h"
#include "Renderer/SceneRenderer.h"

#include "CommandList.h"
#include "CommonRenderResource.h"
#include "Config/DefaultRenderCoreConfig.h"
#include "ExponentialShadowMapRendering.h"
#include "Material/MaterialResource.h"
#include "Math/TransformationMatrix.h"
#include "Mesh/StaticMeshResource.h"
#include "Physics/CollideNarrow.h"
#include "Physics/Frustum.h"
#include "Physics/ICollider.h"
#include "Proxies/LightProxy.h"
#include "Proxies/PrimitiveProxy.h"
#include "Proxies/TexturedSkyProxy.h"
#include "Proxies/VolumetricCloudProxy.h"
#include "RenderTargetPool.h"
#include "RenderView.h"
#include "Scene/IScene.h"
#include "Scene/PrimitiveSceneInfo.h"
#include "Scene/Scene.h"
#include "Scene/SceneConstantBuffers.h"
#include "Scene/VolumetricCloudSceneInfo.h"
#include "Scene/VolumetricFogSceneInfo.h"
#include "ShaderBindings.h"
#include "ShadowDrawPassProcessor.h"
#include "ShadowSetup.h"
#include "SkyAtmosphereRendering.h"
#include "StaticState.h"
#include "TransitionUtils.h"
#include "Viewport.h"
#include "VolumetricCloudPassProcessor.h"
#include "VolumetricFogPassProcessor.h"

#include <deque>

namespace rendercore
{
	void RenderingShaderResource::BindResources( const ShaderStates& shaders, agl::ShaderBindings& bindings )
	{
		const ShaderBase* shaderArray[] = {
			shaders.m_vertexShader,
			nullptr, // HS
			nullptr, // DS
			shaders.m_geometryShader,
			shaders.m_pixelShader,
			nullptr, // CS
		};

		for ( uint32 shaderType = 0; shaderType < agl::MAX_SHADER_TYPE<uint32>; ++shaderType )
		{
			if ( shaderArray[shaderType] == nullptr )
			{
				continue;
			}

			agl::SingleShaderBindings singleBinding = bindings.GetSingleShaderBindings( static_cast<agl::ShaderType>( shaderType ) );

			if ( singleBinding.GetShaderType() == agl::ShaderType::None )
			{
				continue;
			}

			const auto& parameterMap = shaderArray[shaderType]->ParameterMap();

			for ( size_t i = 0; i < m_parameterNames.size(); ++i )
			{
				agl::GraphicsApiResource* resource = m_resources[i];
				if ( resource == nullptr )
				{
					continue;
				}

				agl::ShaderParameter parameter = parameterMap.GetParameter( m_parameterNames[i].Str().data() );

				switch ( parameter.m_type )
				{
				case agl::ShaderParameterType::ConstantBuffer:
					singleBinding.AddConstantBuffer( parameter, reinterpret_cast<agl::Buffer*>( m_resources[i] ) );
					break;
				case agl::ShaderParameterType::SRV:
					singleBinding.AddSRV( parameter, reinterpret_cast<agl::ShaderResourceView*>( m_resources[i] ) );
					break;
				case agl::ShaderParameterType::UAV:
					singleBinding.AddUAV( parameter, reinterpret_cast<agl::UnorderedAccessView*>( m_resources[i] ) );
					break;
				case agl::ShaderParameterType::Sampler:
					singleBinding.AddSampler( parameter, reinterpret_cast<agl::SamplerState*>( m_resources[i] ) );
					break;
				default:
					break;
				}
			}
		}
	}

	void RenderingShaderResource::AddResource( const std::string& parameterName, agl::GraphicsApiResource* resource )
	{
		auto found = std::find( std::begin( m_parameterNames ), std::end( m_parameterNames ), Name( parameterName ) );

		if ( found == std::end( m_parameterNames ) )
		{
			m_parameterNames.emplace_back( parameterName );
			m_resources.emplace_back( resource );
		}
		else
		{
			size_t idx = std::distance( std::begin( m_parameterNames ), found );
			m_resources[idx] = resource;
		}
	}

	void RenderingShaderResource::ClearResources()
	{
		for ( auto& resource : m_resources )
		{
			resource = nullptr;
		}
	}

	bool SceneRenderer::PreRender( RenderViewGroup& renderViewGroup )
	{
		std::construct_at( &m_passSnapshots );
		std::construct_at( &m_shadowInfos );

		for ( auto& view : renderViewGroup )
		{
			PassVisibleSnapshots& passSnapshot = m_passSnapshots.emplace_back();
			view.m_snapshots = passSnapshot.data();
		}

		InitDynamicShadows( renderViewGroup );

		auto linearSampler = StaticSamplerState<>::Get();
		m_shaderResources.AddResource( "LinearSampler", linearSampler.Resource() );
		
		return true;
	}

	void SceneRenderer::PostRender( RenderViewGroup& renderViewGroup )
	{
		if ( DefaultRenderCore::IsTaaEnabled() )
		{
			RenderTemporalAntiAliasing( renderViewGroup );
		}

		m_shaderResources.ClearResources();

		m_prevFrameContext.resize( renderViewGroup.Size() );
		for ( size_t i = 0; i < m_prevFrameContext.size(); ++i )
		{
			const RenderView& view = renderViewGroup[i];

			auto viewMatrix = LookFromMatrix( view.m_viewOrigin
				, view.m_viewAxis[2]
				, view.m_viewAxis[1] );
			m_prevFrameContext[i].m_viewMatrix = viewMatrix;

			auto projMatrix = PerspectiveMatrix( view.m_fov
				, view.m_aspect
				, view.m_nearPlaneDistance
				, view.m_farPlaneDistance );
			m_prevFrameContext[i].m_projMatrix = projMatrix;

			auto viewProjMatrix = viewMatrix * projMatrix;
			m_prevFrameContext[i].m_viewProjMatrix = viewProjMatrix;
		}

		std::destroy_at( &m_passSnapshots );
		std::destroy_at( &m_shadowInfos );

		GetTransientAllocator<ThreadType::RenderThread>().Flush();
	}

	PrimitiveIdVertexBufferPool& SceneRenderer::GetPrimitiveIdPool()
	{
		return m_primitiveIdBufferPool;
	}

	void SceneRenderer::WaitUntilRenderingIsFinish()
	{
		GetPrimitiveIdPool().DiscardAll();
	}

	void SceneRenderer::InitDynamicShadows( RenderViewGroup& renderViewGroup )
	{
		IScene& scene = renderViewGroup.Scene();
		if ( scene.GetRenderScene() == nullptr )
		{
			return;
		}

		RenderThreadFrameData<ShadowInfo*> viewDependentShadow;
		RenderThreadFrameData<ShadowInfo*> viewIndependentShadow;

		Scene& renderScene = *scene.GetRenderScene();
		auto lights = renderScene.Lights();
		m_shadowInfos.reserve( lights.Size() );

		for ( const auto& view : renderViewGroup )
		{
			for ( LightSceneInfo* light : lights )
			{
				LightProxy* proxy = light->Proxy();
				if ( proxy->CastShadow() )
				{
					ShadowInfo& shadowInfo = m_shadowInfos.emplace_back( light, view );

					if ( proxy->GetLightType() == LightType::Directional )
					{
						viewDependentShadow.push_back( &shadowInfo );
					}
					else
					{
						viewIndependentShadow.push_back( &shadowInfo );
					}
				}
			}
		}

		ClassifyViewDependentShadowCasterAndReceiver( scene, viewDependentShadow );
		ClassifyViewIndependentShadowCasterAndReceiver( scene, viewIndependentShadow );

		SetupShadow();

		AllocateShadowMaps();
	}

	void SceneRenderer::ClassifyViewDependentShadowCasterAndReceiver( IScene& scene, const RenderThreadFrameData<ShadowInfo*>& shadows )
	{
		Scene& renderScene = *scene.GetRenderScene();

		for ( ShadowInfo* pShadowInfo : shadows )
		{
			LightSceneInfo* lightSceneInfo = pShadowInfo->GetLightSceneInfo();
			[[maybe_unused]] LightType lightType = pShadowInfo->GetLightType();

			assert( lightType == LightType::Directional );
			assert( pShadowInfo->View() != nullptr );

			const RenderView& view = *pShadowInfo->View();
			auto viewMat = LookFromMatrix( view.m_viewOrigin, view.m_viewAxis[2], view.m_viewAxis[1] );
			auto viewProjectionMat = PerspectiveMatrix( view.m_fov, view.m_aspect, view.m_nearPlaneDistance, view.m_farPlaneDistance );
			viewProjectionMat = viewMat * viewProjectionMat;
			Frustum frustum( viewProjectionMat );

			const Vector& lightDirection = lightSceneInfo->Proxy()->GetLightProperty().m_direction;
			Vector sweepDir = lightDirection.GetNormalized();

			const auto& intersectionInfos = lightSceneInfo->Primitives();
			for ( const auto& intersectionInfo : intersectionInfos )
			{
				PrimitiveSceneInfo* primitive = intersectionInfo.m_primitive;
				uint32 id = primitive->PrimitiveId();

				const BoxSphereBounds& bounds = renderScene.PrimitiveBounds()[id];

				CollisionResult inFrustum = BoxAndFrustum( bounds.Origin() - bounds.HalfSize(),
					bounds.Origin() + bounds.HalfSize(),
					frustum );

				BoxSphereBounds viewspaceBounds = bounds.TransformBy( viewMat );

				switch ( inFrustum )
				{
				case CollisionResult::Outside:
				{
					CollisionResult sweepResult = SphereAndFrusturm( bounds.Origin(), bounds.Radius(), frustum, sweepDir );
					if ( sweepResult > CollisionResult::Outside )
					{
						pShadowInfo->AddCasterPrimitive( primitive, viewspaceBounds );
					}
				}
				break;
				case CollisionResult::Inside:
				case CollisionResult::Intersection:
				{
					pShadowInfo->AddCasterPrimitive( primitive, viewspaceBounds );
					pShadowInfo->AddReceiverPrimitive( primitive, viewspaceBounds );
				}
				break;
				}
			}
		}
	}

	void SceneRenderer::ClassifyViewIndependentShadowCasterAndReceiver( IScene& scene, const RenderThreadFrameData<ShadowInfo*>& shadows )
	{
		for ( ShadowInfo* pShadowInfo : shadows )
		{
			LightSceneInfo* lightSceneInfo = pShadowInfo->GetLightSceneInfo();

			const auto& intersectionInfos = lightSceneInfo->Primitives();
			for ( const auto& intersectionInfo : intersectionInfos )
			{
				PrimitiveSceneInfo* primitive = intersectionInfo.m_primitive;

				pShadowInfo->AddCasterPrimitive( primitive );
			}
		}
	}

	void SceneRenderer::SetupShadow()
	{
		for ( ShadowInfo& shadowInfo : m_shadowInfos )
		{
			if ( shadowInfo.HasCasterPrimitives() == false )
			{
				continue;
			}

			LightType lightType = shadowInfo.GetLightType();

			switch ( lightType )
			{
			case LightType::Directional:
			{
				BuildOrthoShadowProjectionMatrix( shadowInfo );
				break;
			}
			case LightType::Point:
			{
				BuildPointShadowProjectionMatrix( shadowInfo );
				break;
			}
			case LightType::Spot:
				break;
			default:
				break;
			}
		}
	}

	void SceneRenderer::AllocateShadowMaps()
	{
		RenderThreadFrameData<ShadowInfo*> cascadeShadows;
		RenderThreadFrameData<ShadowInfo*> pointShadows;

		for ( auto& shadowInfo : m_shadowInfos )
		{
			LightType lightType = shadowInfo.GetLightType();

			switch ( lightType )
			{
			case LightType::Directional:
				cascadeShadows.push_back( &shadowInfo );
				break;
			case LightType::Point:
				pointShadows.push_back( &shadowInfo );
				break;
			case LightType::Spot:
				break;
			default:
				break;
			}
		}

		if ( cascadeShadows.size() > 0 )
		{
			AllocateCascadeShadowMaps( cascadeShadows );
		}

		if ( pointShadows.size() > 0 )
		{
			AllocatePointShadowMaps( pointShadows );
		}
	}

	void SceneRenderer::AllocateCascadeShadowMaps( const RenderThreadFrameData<ShadowInfo*>& shadows )
	{
		for ( ShadowInfo* shadow : shadows )
		{
			auto [width, height] = shadow->ShadowMapSize();

			agl::TextureTrait trait = {
				.m_width = width,
				.m_height = height,
				.m_depth = CascadeShadowSetting::MAX_CASCADE_NUM, // Cascade map count, Right now, it's fixed constant.
				.m_sampleCount = 1,
				.m_sampleQuality = 0,
				.m_mipLevels = 1,
				.m_format = agl::ResourceFormat::R32_FLOAT,
				.m_access = agl::ResourceAccessFlag::Default,
				.m_bindType = agl::ResourceBindType::RenderTarget | agl::ResourceBindType::ShaderResource,
				.m_miscFlag = agl::ResourceMisc::None,
				.m_clearValue = agl::ResourceClearValue{
					.m_color = { 1.f, 1.f, 1.f, 1.f }
				}
			};

			shadow->ShadowMap().m_shadowMaps.emplace_back( RenderTargetPool::GetInstance().FindFreeRenderTarget( trait, "Shadow.Cascade" ) );

			agl::TextureTrait depthTrait = {
				.m_width = width,
				.m_height = height,
				.m_depth = CascadeShadowSetting::MAX_CASCADE_NUM, // Cascade map count, Right now, it's fixed constant.
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

			shadow->ShadowMap().m_shadowMapDepth = RenderTargetPool::GetInstance().FindFreeRenderTarget( depthTrait, "Shadow.Cascade.Depth" );

			if ( DefaultRenderCore::IsRSMsEnabled() )
			{
				agl::TextureTrait positionMapTrait = {
					.m_width = width,
					.m_height = height,
					.m_depth = CascadeShadowSetting::MAX_CASCADE_NUM, // Cascade map count, Right now, it's fixed constant.
					.m_sampleCount = 1,
					.m_sampleQuality = 0,
					.m_mipLevels = 1,
					.m_format = agl::ResourceFormat::R32G32B32A32_FLOAT,
					.m_access = agl::ResourceAccessFlag::Default,
					.m_bindType = agl::ResourceBindType::RenderTarget | agl::ResourceBindType::ShaderResource,
					.m_miscFlag = agl::ResourceMisc::None,
					.m_clearValue = agl::ResourceClearValue{
						.m_color = { 0.f, 0.f, 0.f, 1.f }
					}
				};
				
				shadow->ShadowMap().m_shadowMaps.emplace_back( RenderTargetPool::GetInstance().FindFreeRenderTarget( positionMapTrait, "RSMs.Position" ) );

				agl::TextureTrait normalMapTrait = {
					.m_width = width,
					.m_height = height,
					.m_depth = CascadeShadowSetting::MAX_CASCADE_NUM, // Cascade map count, Right now, it's fixed constant.
					.m_sampleCount = 1,
					.m_sampleQuality = 0,
					.m_mipLevels = 1,
					.m_format = agl::ResourceFormat::R10G10B10A2_UNORM,
					.m_access = agl::ResourceAccessFlag::Default,
					.m_bindType = agl::ResourceBindType::RenderTarget | agl::ResourceBindType::ShaderResource,
					.m_miscFlag = agl::ResourceMisc::None,
					.m_clearValue = agl::ResourceClearValue{
						.m_color = { 0.f, 0.f, 0.f, 1.f }
					}
				};

				shadow->ShadowMap().m_shadowMaps.emplace_back( RenderTargetPool::GetInstance().FindFreeRenderTarget( normalMapTrait, "RSMs.Normal" ) );

				agl::TextureTrait fluxMapTrait = {
					.m_width = width,
					.m_height = height,
					.m_depth = CascadeShadowSetting::MAX_CASCADE_NUM, // Cascade map count, Right now, it's fixed constant.
					.m_sampleCount = 1,
					.m_sampleQuality = 0,
					.m_mipLevels = 1,
					.m_format = agl::ResourceFormat::R8G8B8A8_UNORM,
					.m_access = agl::ResourceAccessFlag::Default,
					.m_bindType = agl::ResourceBindType::RenderTarget | agl::ResourceBindType::ShaderResource,
					.m_miscFlag = agl::ResourceMisc::None,
					.m_clearValue = agl::ResourceClearValue{
						.m_color = { 0.f, 0.f, 0.f, 1.f }
					}
				};

				shadow->ShadowMap().m_shadowMaps.emplace_back( RenderTargetPool::GetInstance().FindFreeRenderTarget( fluxMapTrait, "RSMs.Flux" ) );
			}
		}
	}

	void SceneRenderer::AllocatePointShadowMaps( const RenderThreadFrameData<ShadowInfo*>& shadows )
	{
		for ( ShadowInfo* shadow : shadows )
		{
			auto [width, height] = shadow->ShadowMapSize();

			agl::TextureTrait trait = {
				.m_width = width,
				.m_height = height,
				.m_depth = 6,
				.m_sampleCount = 1,
				.m_sampleQuality = 0,
				.m_mipLevels = 1,
				.m_format = agl::ResourceFormat::R32_FLOAT,
				.m_access = agl::ResourceAccessFlag::Default,
				.m_bindType = agl::ResourceBindType::RenderTarget | agl::ResourceBindType::ShaderResource,
				.m_miscFlag = agl::ResourceMisc::TextureCube,
				.m_clearValue = agl::ResourceClearValue{
					.m_color = { 1.f, 1.f, 1.f, 1.f }
				}
			};

			shadow->ShadowMap().m_shadowMaps.emplace_back( RenderTargetPool::GetInstance().FindFreeRenderTarget( trait, "Shadow.Point" ) );

			agl::TextureTrait depthTrait = {
				.m_width = width,
				.m_height = height,
				.m_depth = 6,
				.m_sampleCount = 1,
				.m_sampleQuality = 0,
				.m_mipLevels = 1,
				.m_format = agl::ResourceFormat::D24_UNORM_S8_UINT,
				.m_access = agl::ResourceAccessFlag::Default,
				.m_bindType = agl::ResourceBindType::DepthStencil,
				.m_miscFlag = agl::ResourceMisc::TextureCube,
				.m_clearValue = agl::ResourceClearValue{
					.m_depthStencil = {
						.m_depth = 1.f,
						.m_stencil = 0
					}
				}
			};

			shadow->ShadowMap().m_shadowMapDepth = RenderTargetPool::GetInstance().FindFreeRenderTarget( depthTrait, "Shadow.Point.Depth" );
		}
	}

	void SceneRenderer::RenderShadowDepthPass()
	{
		for ( ShadowInfo& shadowInfo : m_shadowInfos )
		{
			ShadowMapRenderTarget& shadowMap = shadowInfo.ShadowMap();
			assert( ( shadowMap.m_shadowMaps.size() > 0 )
				&& ( shadowMap.m_shadowMaps[0] != nullptr ) 
				&& ( shadowMap.m_shadowMapDepth != nullptr ));

			auto [width, height] = shadowInfo.ShadowMapSize();

			RenderingOutputContext context = {
				.m_renderTargets = {},
				.m_depthStencil = shadowMap.m_shadowMapDepth,
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

			for ( int32 i = 0; i < shadowMap.m_shadowMaps.size(); ++i )
			{
				context.m_renderTargets[i] = shadowMap.m_shadowMaps[i];
			}

			StoreOuputContext( context );

			auto commandList = GetCommandList();

			using TransitionArray = std::vector<agl::ResourceTransition, InlineAllocator<agl::ResourceTransition, 8>>;
			TransitionArray beforeRenderDepth;

			beforeRenderDepth.push_back( Transition( *shadowMap.m_shadowMaps[0].Get(), agl::ResourceState::RenderTarget ) );
			beforeRenderDepth.push_back( Transition( *shadowMap.m_shadowMapDepth.Get(), agl::ResourceState::DepthWrite ) );

			bool rsmsEnabled = DefaultRenderCore::IsRSMsEnabled() && shadowMap.m_shadowMaps.size() >= 4;
			if ( rsmsEnabled )
			{
				beforeRenderDepth.push_back( Transition( *shadowMap.m_shadowMaps[1].Get(), agl::ResourceState::RenderTarget ) );
				beforeRenderDepth.push_back( Transition( *shadowMap.m_shadowMaps[2].Get(), agl::ResourceState::RenderTarget ) );
				beforeRenderDepth.push_back( Transition( *shadowMap.m_shadowMaps[3].Get(), agl::ResourceState::RenderTarget ) );
			}

			commandList.Transition( static_cast<uint32>( beforeRenderDepth.size() ), beforeRenderDepth.data());

			agl::RenderTargetView* rtv = shadowMap.m_shadowMaps[0]->RTV();
			commandList.ClearRenderTarget( rtv, { 1, 1, 1, 1 } );

			if ( rsmsEnabled )
			{
				rtv = shadowMap.m_shadowMaps[1]->RTV();
				commandList.ClearRenderTarget( rtv, { 0, 0, 0, 1 } );

				rtv = shadowMap.m_shadowMaps[2]->RTV();
				commandList.ClearRenderTarget( rtv, { 0, 0, 0, 1 } );

				rtv = shadowMap.m_shadowMaps[3]->RTV();
				commandList.ClearRenderTarget( rtv, { 0, 0, 0, 1 } );
			}

			agl::DepthStencilView* dsv = shadowMap.m_shadowMapDepth->DSV();
			commandList.ClearDepthStencil( dsv, 1.f, 0 );

			shadowInfo.SetupShadowConstantBuffer();
			shadowInfo.RenderDepth( *this, m_shaderResources );

			TransitionArray afterRenderDepth;

			afterRenderDepth.push_back( Transition( *shadowMap.m_shadowMaps[0].Get(), agl::ResourceState::GenericRead ) );

			if ( rsmsEnabled )
			{
				afterRenderDepth.push_back( Transition( *shadowMap.m_shadowMaps[1].Get(), agl::ResourceState::GenericRead ) );
				afterRenderDepth.push_back( Transition( *shadowMap.m_shadowMaps[2].Get(), agl::ResourceState::GenericRead ) );
				afterRenderDepth.push_back( Transition( *shadowMap.m_shadowMaps[3].Get(), agl::ResourceState::GenericRead ) );
			}

			commandList.Transition( static_cast<uint32>( afterRenderDepth.size() ), afterRenderDepth.data() );

			if ( DefaultRenderCore::IsESMsEnabled() )
			{
				shadowMap.m_shadowMaps[0] = GenerateExponentialShadowMaps( shadowInfo, shadowMap.m_shadowMaps[0] );
			}
		}
	}

	void SceneRenderer::RenderTexturedSky( IScene& scene )
	{
		Scene* renderScene = scene.GetRenderScene();
		if ( renderScene == nullptr )
		{
			return;
		}

		TexturedSkyProxy* proxy = renderScene->TexturedSky();
		if ( proxy == nullptr )
		{
			return;
		}

		StaticMeshRenderData* renderData = proxy->GetRenderData();
		MaterialResource* material = proxy->GetMaterialResource();
		if ( ( renderData == nullptr ) || ( renderData->LODSize() == 0 || ( material == nullptr ) ) )
		{
			return;
		}

		ShaderStates shaderState{
			{},
			material->GetVertexShader(),
			nullptr,
			material->GetPixelShader(),
		};

		auto commandList = GetCommandList();
		ApplyOutputContext( commandList );

		StaticMeshLODResource& lodResource = renderData->LODResource( 0 );
		const VertexCollection& vertexCollection = lodResource.m_vertexCollection;

		for ( const auto& section : lodResource.m_sections )
		{
			DrawSnapshot snapshot;
			vertexCollection.Bind( snapshot.m_vertexStream, VertexStreamLayoutType::PositionOnly );
			snapshot.m_primitiveIdSlot = -1;
			snapshot.m_indexBuffer = lodResource.m_ib;

			GraphicsPipelineState& pipelineState = snapshot.m_pipelineState;
			pipelineState.m_shaderState = shaderState;

			auto initializer = CreateShaderBindingsInitializer( pipelineState.m_shaderState );
			snapshot.m_shaderBindings.Initialize( initializer );

			material->TakeSnapshot( snapshot );

			auto& graphicsInterface = GraphicsInterface();
			if ( pipelineState.m_shaderState.m_vertexShader )
			{
				VertexStreamLayout vertexlayout = vertexCollection.VertexLayout( VertexStreamLayoutType::PositionOnly );
				pipelineState.m_shaderState.m_vertexLayout = graphicsInterface.FindOrCreate( *pipelineState.m_shaderState.m_vertexShader, vertexlayout );
			}

			pipelineState.m_depthStencilState = graphicsInterface.FindOrCreate( proxy->GetDepthStencilOption() );
			pipelineState.m_rasterizerState = graphicsInterface.FindOrCreate( proxy->GetRasterizerOption() );

			pipelineState.m_primitive = agl::ResourcePrimitive::Trianglelist;

			snapshot.m_count = section.m_count;
			snapshot.m_startIndexLocation = section.m_startLocation;
			snapshot.m_baseVertexLocation = 0;

			PreparePipelineStateObject( snapshot );

			m_shaderResources.BindResources( pipelineState.m_shaderState, snapshot.m_shaderBindings );

			AddSingleDrawPass( snapshot );
		}
	}

	void SceneRenderer::RenderMesh( IScene& scene, RenderPass passType, RenderView& renderView )
	{
		const auto& primitives = scene.Primitives();
		if ( primitives.Size() == 0 )
		{
			return;
		}

		auto* renderScene = scene.GetRenderScene();
		if ( renderScene == nullptr )
		{
			return;
		}

		auto& snapshots = renderView.m_snapshots[static_cast<uint32>( passType )];

		std::deque<DrawSnapshot> snapshotStorage;

		// Create DrawSnapshot
		for ( auto primitive : primitives )
		{
			PrimitiveProxy* proxy = primitive->Proxy();

			const std::vector<PrimitiveSubMeshInfo>& subMeshInfos = primitive->SubMeshInfos();

			if ( subMeshInfos.size() > 0 )
			{
				for ( const auto& subMeshInfo : subMeshInfos )
				{
					auto snapshotIndex = subMeshInfo.GetCachedDrawSnapshotInfoIndex( passType );
					if ( snapshotIndex )
					{
						const CachedDrawSnapshotInfo& info = primitive->GetCachedDrawSnapshotInfo( *snapshotIndex );
						DrawSnapshot& snapshot = primitive->CachedDrawSnapshot( *snapshotIndex );

						VisibleDrawSnapshot& visibleSnapshot = snapshots.emplace_back();
						visibleSnapshot.m_snapshotBucketId = info.m_snapshotBucketId;
						visibleSnapshot.m_drawSnapshot = &snapshot;
						visibleSnapshot.m_primitiveId = proxy->PrimitiveId();
						visibleSnapshot.m_numInstance = 1;
					}
				}
			}
			else
			{
				proxy->TakeSnapshot( snapshotStorage, snapshots );
			}
		}

		if ( snapshots.size() == 0 )
		{
			return;
		}

		// Update invalidated resources
		for ( auto& viewDrawSnapshot : snapshots )
		{
			DrawSnapshot& snapshot = *viewDrawSnapshot.m_drawSnapshot;
			GraphicsPipelineState& pipelineState = snapshot.m_pipelineState;

			m_shaderResources.BindResources( pipelineState.m_shaderState, snapshot.m_shaderBindings );
		}

		VertexBuffer primitiveIds = GetPrimitiveIdPool().Alloc( static_cast<uint32>( snapshots.size() * sizeof( uint32 ) ) );

		SortDrawSnapshots( snapshots, primitiveIds );
		// CommitDrawSnapshots( *this, renderViewGroup, curView, primitiveIds );
		ParallelCommitDrawSnapshot( *this, snapshots, primitiveIds );
	}

	void SceneRenderer::RenderShadow()
	{
		for ( ShadowInfo& shadowInfo : m_shadowInfos )
		{
			std::optional<DrawSnapshot> result;

			switch ( shadowInfo.GetLightType() )
			{
			case LightType::Directional:
			{
				CascadeShadowDrawPassProcessor shadowDrawPassProcessor;
				result = shadowDrawPassProcessor.Process( FullScreenQuadDrawInfo() );
				break;
			}
			case LightType::Point:
			{
				PointShadowDrawPassProcessor shadowDrawPassProcessor;
				result = shadowDrawPassProcessor.Process( FullScreenQuadDrawInfo() );
				break;
			}
			}

			if ( result.has_value() == false )
			{
				return;
			}

			DrawSnapshot& snapshot = *result;

			// Update invalidated resources
			GraphicsPipelineState& pipelineState = snapshot.m_pipelineState;
			m_shaderResources.BindResources( pipelineState.m_shaderState, snapshot.m_shaderBindings );

			auto commandList = GetCommandList();
			ApplyOutputContext( commandList );

			m_shaderResources.AddResource( "ShadowTexture", shadowInfo.ShadowMap().m_shadowMaps[0]->SRV() );

			bool bESMsEnabled = DefaultRenderCore::IsESMsEnabled();

			SamplerState shadowSampler;
			if ( ( bESMsEnabled == false ) || ( shadowInfo.GetLightType() != LightType::Directional ) )
			{
				shadowSampler = StaticSamplerState<agl::TextureFilter::MinMagMipLinear | agl::TextureFilter::Comparison
					, agl::TextureAddressMode::Border
					, agl::TextureAddressMode::Border
					, agl::TextureAddressMode::Border
					, 0.f
					, agl::ComparisonFunc::Less>::Get();
			}
			else
			{
				shadowSampler = StaticSamplerState<>::Get();
			}
			
			m_shaderResources.AddResource( "ShadowSampler", shadowSampler.Resource() );

			m_shaderResources.AddResource( "ShadowDepthPassParameters", shadowInfo.ConstantBuffer().Resource() );

			if ( bESMsEnabled )
			{
				m_shaderResources.AddResource( "ESMsParameters", shadowInfo.ESMsConstantBuffer().Resource());
			}

			m_shaderResources.BindResources( pipelineState.m_shaderState, snapshot.m_shaderBindings );

			AddSingleDrawPass( snapshot );
		}
	}

	void SceneRenderer::RenderSkyAtmosphere( IScene& scene, RenderView& renderView )
	{
		Scene* renderScene = scene.GetRenderScene();
		if ( renderScene == nullptr )
		{
			return;
		}

		SkyAtmosphereRenderSceneInfo* info = renderScene->SkyAtmosphereSceneInfo();
		if ( info == nullptr )
		{
			return;
		}

		const LightSceneInfo* skyAtmosphereLight = renderScene->SkyAtmosphereSunLight();
		if ( skyAtmosphereLight == nullptr )
		{
			return;
		}

		LightProperty lightProperty = skyAtmosphereLight->Proxy()->GetLightProperty();

		auto& skyAtmosphereRenderParameter = info->GetSkyAtmosphereRenderParameter();
		SkyAtmosphereRenderParameter param = {
			.m_cameraPos = renderView.m_viewOrigin,
			.m_sunDir = -lightProperty.m_direction,
			.m_exposure = 0.4f,
		};
		skyAtmosphereRenderParameter.Update( param );

		SkyAtmosphereDrawPassProcessor skyAtmosphereDrawPassProcessor;
		auto result = skyAtmosphereDrawPassProcessor.Process( FullScreenQuadDrawInfo() );
		if ( result.has_value() == false )
		{
			return;
		}

		DrawSnapshot& snapshot = *result;

		// Update invalidated resources
		GraphicsPipelineState& pipelineState = snapshot.m_pipelineState;
		m_shaderResources.BindResources( pipelineState.m_shaderState, snapshot.m_shaderBindings );

		auto commandList = GetCommandList();
		ApplyOutputContext( commandList );

		auto linearSampler = StaticSamplerState<>::Get();

		RenderingShaderResource skyAtmosphereDrawResources;
		skyAtmosphereDrawResources.AddResource( "TransmittanceLut", info->GetTransmittanceLutTexture()->SRV() );
		skyAtmosphereDrawResources.AddResource( "TransmittanceLutSampler", linearSampler.Resource() );
		skyAtmosphereDrawResources.AddResource( "IrradianceLut", info->GetIrradianceLutTexture()->SRV() );
		skyAtmosphereDrawResources.AddResource( "IrradianceLutSampler", linearSampler.Resource() );
		skyAtmosphereDrawResources.AddResource( "InscatterLut", info->GetInscatterLutTexture()->SRV() );
		skyAtmosphereDrawResources.AddResource( "InscatterLutSampler", linearSampler.Resource() );
		skyAtmosphereDrawResources.AddResource( "SkyAtmosphereRenderParameter", skyAtmosphereRenderParameter.Resource() );

		skyAtmosphereDrawResources.BindResources( pipelineState.m_shaderState, snapshot.m_shaderBindings );

		AddSingleDrawPass( snapshot );
	}

	void SceneRenderer::RenderVolumetricCloud( IScene& scene, [[maybe_unused]] RenderView& renderView )
	{
		Scene* renderScene = scene.GetRenderScene();
		if ( renderScene == nullptr )
		{
			return;
		}

		VolumetricCloudSceneInfo* info = renderScene->VolumetricCloud();
		if ( info == nullptr )
		{
			return;
		}

		info->CreateRenderData();

		const LightSceneInfo* skyAtmosphereLight = renderScene->SkyAtmosphereSunLight();
		if ( skyAtmosphereLight == nullptr )
		{
			return;
		}

		VolumetricCloundDrawPassProcessor volumetricCloundDrawPassProcessor;
		auto result = volumetricCloundDrawPassProcessor.Process( FullScreenQuadDrawInfo() );
		if ( result.has_value() == false )
		{
			return;
		}

		Vector4 lightPosOrDir;
		LightProperty lightProperty = skyAtmosphereLight->Proxy()->GetLightProperty();
		if ( lightProperty.m_type == LightType::Directional )
		{
			lightPosOrDir = Vector4( -lightProperty.m_direction.x, -lightProperty.m_direction.y, -lightProperty.m_direction.z, 0.f );
		}
		else
		{
			lightPosOrDir = Vector4( lightProperty.m_position.x, lightProperty.m_position.y, lightProperty.m_position.z, 1.f );
		}

		const VolumetricCloudProxy& proxy = *info->Proxy();
		VolumetricCloudRenderParameter param = {
			.m_sphereRadius = Vector( proxy.EarthRadius(), proxy.InnerRadius(), proxy.OuterRadius() ),
			.m_lightAbsorption = proxy.LightAbsorption(),
			.m_lightPosOrDir = lightPosOrDir,
			.m_cloudColor = proxy.CloudColor(),
			.m_windDirection = Vector( 0.5f, 0.f, 0.1f ).GetNormalized(),
			.m_windSpeed = 450.f,
			.m_crispiness = proxy.Crispiness(),
			.m_curliness = proxy.Curliness(),
			.m_densityFactor = proxy.DensityFactor(),
			.m_densityScale = proxy.DensityScale(),
		};

		auto& volumetricCloudRenderParameter = info->GetVolumetricCloudRenderParameter();
		volumetricCloudRenderParameter.Update( param );

		DrawSnapshot& snapshot = *result;

		// Update invalidated resources
		GraphicsPipelineState& pipelineState = snapshot.m_pipelineState;
		m_shaderResources.BindResources( pipelineState.m_shaderState, snapshot.m_shaderBindings );

		auto commandList = GetCommandList();
		ApplyOutputContext( commandList );

		SamplerState wrapSamplerState = StaticSamplerState<agl::TextureFilter::MinMagMipLinear
			, agl::TextureAddressMode::Wrap
			, agl::TextureAddressMode::Wrap
			, agl::TextureAddressMode::Wrap>::Get();

		RenderingShaderResource volumetricCloundDrawResources;
		volumetricCloundDrawResources.AddResource( "VolumetricCloudRenderParameter", volumetricCloudRenderParameter.Resource() );
		volumetricCloundDrawResources.AddResource( "BaseCloudShape", info->BaseCloudShape()->SRV() );
		volumetricCloundDrawResources.AddResource( "DetailCloudShape", info->DetailCloudShape()->SRV() );
		volumetricCloundDrawResources.AddResource( "WeatherMap", info->WeatherMap()->SRV() );
		volumetricCloundDrawResources.AddResource( "BaseCloudSampler", wrapSamplerState.Resource() );
		volumetricCloundDrawResources.AddResource( "WeatherSampler", wrapSamplerState.Resource() );

		volumetricCloundDrawResources.BindResources( pipelineState.m_shaderState, snapshot.m_shaderBindings );

		AddSingleDrawPass( snapshot );
	}

	void SceneRenderer::RenderVolumetricFog( IScene& scene, [[maybe_unused]] RenderView& renderView )
	{
		Scene* renderScene = scene.GetRenderScene();
		if ( renderScene == nullptr )
		{
			return;
		}

		VolumetricFogSceneInfo* info = renderScene->VolumetricFog();
		if ( info == nullptr )
		{
			return;
		}

		info->CreateRenderData();
		info->UpdateParameter();
		info->PrepareFrustumVolume( *renderScene, m_forwardLighting, m_shadowInfos );

		VolumetricFogDrawPassProcessor volumetricFogDrawPassProcessor;
		auto result = volumetricFogDrawPassProcessor.Process( FullScreenQuadDrawInfo() );
		if ( result.has_value() == false )
		{
			return;
		}

		DrawSnapshot& snapshot = *result;

		// Update invalidated resources
		GraphicsPipelineState& pipelineState = snapshot.m_pipelineState;
		m_shaderResources.BindResources( pipelineState.m_shaderState, snapshot.m_shaderBindings );

		auto commandList = GetCommandList();
		ApplyOutputContext( commandList );

		SamplerState accumulatedVolumeSampler = StaticSamplerState<>::Get();

		RenderingShaderResource volumetricFogDrawResources;
		volumetricFogDrawResources.AddResource( "AccumulatedVolume", info->AccumulatedVolume()->SRV() );
		volumetricFogDrawResources.AddResource( "AccumulatedVolumeSampler", accumulatedVolumeSampler.Resource() );
		volumetricFogDrawResources.AddResource( "VolumetricFogParameterBuffer", info->GetVolumetricFogParameter().Resource() );

		volumetricFogDrawResources.BindResources( pipelineState.m_shaderState, snapshot.m_shaderBindings );

		AddSingleDrawPass( snapshot );
	}

	void SceneRenderer::RenderTemporalAntiAliasing( RenderViewGroup& renderViewGroup )
	{
		m_taa.Render( GetRenderRenderTargets(), renderViewGroup );
	}

	void SceneRenderer::RenderIndirectIllumination( RenderViewGroup& renderViewGroup )
	{
		m_shaderResources.AddResource( "IndirectIllumination", BlackTexture->SRV() );

		if ( DefaultRenderCore::IsLpvEnabled() )
		{
			m_lpv.Prepare();

			for ( ShadowInfo& shadowInfo : m_shadowInfos )
			{
				const ShadowMapRenderTarget& shadowMapRT = shadowInfo.ShadowMap();
				if ( shadowMapRT.m_shadowMaps.size() < 4 )
				{
					continue;
				}

				const LightSceneInfo* lightSceneInfo = shadowInfo.GetLightSceneInfo();
				if ( lightSceneInfo == nullptr )
				{
					continue;
				}

				IScene& scene = renderViewGroup.Scene();

				LightInjectionParameters params =
				{
					.lightInfo = lightSceneInfo,
					.m_sceneViewParameters = scene.SceneViewConstant().Resource(),
					.m_shadowDepthPassParameters = shadowInfo.ConstantBuffer().Resource(),
					.m_rsmTextures = {
						.m_worldPosition = shadowMapRT.m_shadowMaps[1],
						.m_normal = shadowMapRT.m_shadowMaps[2],
						.m_flux = shadowMapRT.m_shadowMaps[3],
					},
				};

				m_lpv.AddLight( params );
			}

			m_lpv.Propagate();
		}
		else if ( DefaultRenderCore::IsRSMsEnabled() )
		{
			RSMsRenderingParam renderingParam = {
				.m_viewSpaceDistance = GetRenderRenderTargets().GetViewSpaceDistance(),
				.m_worldNormal = GetRenderRenderTargets().GetWorldNormal(),
				.m_shadowInfos = m_shadowInfos.data(),
				.m_numShadowInfos = static_cast<int32>( m_shadowInfos.size() )
			};

			if ( renderingParam.m_viewSpaceDistance == nullptr )
			{
				return;
			}

			if ( ( renderingParam.m_shadowInfos == nullptr )
				|| ( renderingParam.m_numShadowInfos == 0 ) )
			{
				return;
			}

			bool valid = false;
			for ( int32 i = 0; i < renderingParam.m_numShadowInfos; ++i )
			{
				const ShadowInfo& shadowInfo = renderingParam.m_shadowInfos[i];

				const ShadowMapRenderTarget& smrt = shadowInfo.ShadowMap();
				if ( smrt.m_shadowMaps.size() >= 4 )
				{
					valid = true;
					break;
				}
			}

			if ( valid == false )
			{
				return;
			}

			auto renderTarget = renderViewGroup.GetViewport().Texture();
			if ( ( renderTarget == nullptr ) 
				|| ( renderTarget->RTV() == nullptr ))
			{
				return;
			}

			auto [width, height] = renderViewGroup.GetViewport().Size();
			CubeArea<float> viewport = {
					.m_left = 0.f,
					.m_top = 0.f,
					.m_front = 0.f,
					.m_right = static_cast<float>( width ),
					.m_bottom = static_cast<float>( height ),
					.m_back = 1.f
			};

			RectangleArea<int32> scissorRect = {
					.m_left = 0L,
					.m_top = 0L,
					.m_right = static_cast<int32>( width ),
					.m_bottom = static_cast<int32>( height )
			};

			auto rtv = renderTarget->RTV();

			auto commandList = GetCommandList();
			commandList.SetViewports( 1, &viewport );
			commandList.SetScissorRects( 1, &scissorRect );
			commandList.BindRenderTargets( &rtv, 1, nullptr);

			m_rsms.PreRender( renderViewGroup );
			m_rsms.Render( renderingParam, m_shaderResources );
		}
	}

	void SceneRenderer::StoreOuputContext( const RenderingOutputContext& context )
	{
		m_outputContext = context;
	}

	void AddSingleDrawPass( DrawSnapshot& snapshot )
	{
		VisibleDrawSnapshot visibleSnapshot = {
			.m_primitiveId = 0,
			.m_primitiveIdOffset = 0,
			.m_numInstance = 1,
			.m_snapshotBucketId = -1,
			.m_drawSnapshot = &snapshot,
		};

		auto commandList = GetCommandList();

		VertexBuffer emptyPrimitiveID;
		CommitDrawSnapshot( commandList, visibleSnapshot, emptyPrimitiveID );
	}
}
