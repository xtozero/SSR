#include "Renderer/SceneRenderer.h"

#include "CommandList.h"
#include "CommonRenderResource.h"
#include "Config/DefaultRenderCoreConfig.h"
#include "CpuProfiler/CpuProfiler.h"
#include "ExponentialShadowMapRendering.h"
#include "GpuProfiler.h"
#include "GraphicsResourcePool.h"
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
#include "RenderGraph.h"
#include "RenderView.h"
#include "ResourceBarrierUtils.h"
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
#include "Viewport.h"
#include "VolumetricCloudPassProcessor.h"
#include "VolumetricFogPassProcessor.h"

namespace
{
	agl::ShaderParameter HitProxyIdShaderParam( agl::ShaderType::Pixel, agl::ShaderParameterType::ConstantBufferValue, 0, 0, 0, sizeof( ColorF ) );
}

namespace rendercore
{
	void RendererResourceCollection::Clear()
	{
		m_ssgi = BlackTexture;
		m_indirectIllumination = BlackTexture;
	}

	void ResourceBinder::Bind( const ShaderStates& shaders, agl::ShaderBindings& outBindings ) const
	{
		const ShaderBase* shaderArray[] = {
			shaders.m_vertexShader,
			nullptr, // Hull
			nullptr, // Domain
			shaders.m_geometryShader,
			shaders.m_pixelShader,
			nullptr, // Compute
			shaders.m_meshShader,
			shaders.m_amplificationShader,
			nullptr, // RayGen
			nullptr, // Intersection
			nullptr, // AnyHit
			nullptr, // ClosestHit
			nullptr, // Miss
			nullptr, // Callable
		};

		Bind( shaderArray, outBindings );
	}

	void ResourceBinder::Bind( const ComputeShader* shader, agl::ShaderBindings& outBindings ) const
	{
		const ShaderBase* shaderArray[] = {
			nullptr, // Vertex
			nullptr, // Hull
			nullptr, // Domain
			nullptr, // Geometry
			nullptr, // Pixel
			shader,
			nullptr, // Mesh
			nullptr, // Amplification
			nullptr, // RayGen
			nullptr, // Intersection
			nullptr, // AnyHit
			nullptr, // ClosestHit
			nullptr, // Miss
			nullptr, // Callable
		};

		Bind( shaderArray, outBindings );
	}

	void ResourceBinder::Add( const Name& parameterName, agl::GraphicsApiResource* resource )
	{
		auto found = std::ranges::find( m_parameterNames, parameterName );

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

	void ResourceBinder::Add( const ShaderArguments* collection )
	{
		auto found = std::ranges::find( m_argumentsList, collection );
		if ( found != std::end( m_argumentsList ) )
		{
			return;
		}

		m_argumentsList.emplace_back( collection );
	}

	void ResourceBinder::Clear()
	{
		for ( auto& resource : m_resources )
		{
			resource = nullptr;
		}
	}

	void ResourceBinder::Bind( const ShaderBase* (&shaders)[agl::NumShaderTypes<uint32>], agl::ShaderBindings& outBindings ) const
	{
		CPU_PROFILE( ResourceBinder_Bind );

		for ( uint32 shaderType = 0; shaderType < agl::NumShaderTypes<uint32>; ++shaderType )
		{
			if ( shaders[shaderType] == nullptr )
			{
				continue;
			}

			agl::SingleShaderBindings singleBinding = outBindings.GetSingleShaderBindings( static_cast<agl::ShaderType>( shaderType ) );

			if ( singleBinding.GetShaderType() == agl::ShaderType::None )
			{
				continue;
			}

			const auto& parameterMap = shaders[shaderType]->ParameterMap();

			for ( size_t i = 0; i < m_parameterNames.size(); ++i )
			{
				agl::GraphicsApiResource* resource = m_resources[i];
				if ( resource == nullptr )
				{
					continue;
				}

				agl::ShaderParameter parameter = parameterMap.GetParameter( m_parameterNames[i] );

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
				case agl::ShaderParameterType::Bindless:
					singleBinding.AddBindless( parameter, m_resources[i] );
					break;
				default:
					break;
				}
			}

			for ( const ShaderArguments* arguments : m_argumentsList )
			{
				if ( arguments == nullptr )
				{
					continue;
				}

				arguments->Bind( parameterMap, singleBinding );
			}
		}
	}

	void RasterOutput::SetRenderTarget( int32 index, RenderGraphTexture* renderTarget, RasterOutputLoadAction loadAction )
	{
		m_renderTargets[index].m_texture = renderTarget;
		m_renderTargets[index].m_loadAction = loadAction;
	}

	void RasterOutput::SetDepthStencil( RenderGraphTexture* depthStencil, bool readOnly, RasterOutputLoadAction loadAction )
	{
		m_depthStencil.m_texture = depthStencil;
		m_depthStencil.m_loadAction = loadAction;
		m_depthStencilReadOnly = readOnly;
	}

	void RasterOutput::SetViewport( int32 left, int32 top, int32 right, int32 bottom, int32 front, int32 back )
	{
		m_viewport = {
				.m_left = static_cast<float>( left ),
				.m_top = static_cast<float>( top ),
				.m_front = static_cast<float>( front ),
				.m_right = static_cast<float>( right ),
				.m_bottom = static_cast<float>( bottom ),
				.m_back = static_cast<float>( back )
		};
	}

	void RasterOutput::SetViewport( int32 width, int32 height )
	{
		SetViewport( 0, 0, width, height, 0, 1 );
	}

	void RasterOutput::SetScissorRect( int32 left, int32 top, int32 right, int32 bottom )
	{
		m_scissorRect = {
				.m_left = left,
				.m_top = top,
				.m_right = right,
				.m_bottom = bottom
		};
	}

	void RasterOutput::SetScissorRect( int32 width, int32 height )
	{
		SetScissorRect( 0, 0, width, height );
	}

	void RasterOutput::Bind( CommandList& commandList ) const
	{
		agl::RenderTargetView* rtvs[agl::MaxRendertagets] = {};
		agl::DepthStencilView* dsv = nullptr;

		for ( uint32 i = 0; i < agl::MaxRendertagets; ++i )
		{
			if ( m_renderTargets[i].m_texture )
			{
				rtvs[i] = m_renderTargets[i].m_texture->Get()->RTV();
			}
		}

		if ( m_depthStencil.m_texture )
		{
			dsv = m_depthStencil.m_texture->Get()->DSV();
		}

		commandList.BindRenderTargets( rtvs, agl::MaxRendertagets, dsv );
		commandList.SetViewports( 1, &m_viewport );
		commandList.SetScissorRects( 1, &m_scissorRect );
	}

	void SceneRenderer::PreRender( RenderGraph& renderGraph, RenderViewGroup& renderViewGroup )
	{
		ResetFrameData();

		m_prevFrameContext.resize( m_viewInfo.size() );
		for ( size_t i = 0; i < m_prevFrameContext.size(); ++i )
		{
			const RenderViewInfo& viewInfo = m_viewInfo[i];

			m_prevFrameContext[i].m_viewMatrix = viewInfo.m_viewMatrix;
			m_prevFrameContext[i].m_projMatrix = viewInfo.m_projMatrix;
			m_prevFrameContext[i].m_viewProjMatrix = viewInfo.m_viewProjMatrix;
		}

		m_viewInfo.clear();
		m_viewInfo.reserve( renderViewGroup.NumRenderView() );

		IScene& scene = renderViewGroup.Scene();

		assert( scene.GetRenderScene() != nullptr );
		Scene& renderScene = *scene.GetRenderScene();

		for ( size_t i = 0; i < renderViewGroup.NumRenderView(); ++i )
		{
			RenderViewInfo& viewInfo = m_viewInfo.emplace_back( renderViewGroup.GetRenderView( i ) );

			viewInfo.m_viewMatrix = LookFromMatrix( viewInfo.m_viewOrigin
				, viewInfo.m_viewAxis[2]
				, viewInfo.m_viewAxis[1] );

			viewInfo.m_projMatrix = PerspectiveMatrix( viewInfo.m_fov
				, viewInfo.m_aspect
				, viewInfo.m_nearPlaneDistance
				, viewInfo.m_farPlaneDistance );

			viewInfo.m_viewProjMatrix = viewInfo.m_viewMatrix * viewInfo.m_projMatrix;

			PassVisibleDrawSnapshots& passSnapshots = m_drawSnapshotsByView.emplace_back();
			viewInfo.m_drawSnapshots = passSnapshots.data();

			viewInfo.m_visibilityMap.resize( renderScene.Primitives().GetMaxIndex(), true );

			if ( viewInfo.m_state )
			{
				++viewInfo.m_state->m_occlusionFrameCounter;
				viewInfo.m_state->m_occlutionTestPool.Prepare( viewInfo.m_state->m_occlusionFrameCounter );
			}
		}

		m_resourceBinder.Clear();
		m_resourceCollection.Clear();

		auto linearSampler = StaticSamplerState<>::Get();
		m_resourceBinder.Add( StaticName( "LinearSampler" ), linearSampler.Resource() );
		m_resourceBinder.Add( &scene.GetViewShaderArguments() );

		UpdateGPUPrimitiveInfos( renderGraph, renderScene );

		auto& gpuPrimitiveInfo = scene.GpuPrimitiveInfo();
		auto commandList = GetCommandList();

		if ( gpuPrimitiveInfo.Resource() )
		{
			commandList.AddTransition( Transition( *gpuPrimitiveInfo.Resource(), agl::ResourceState::GenericRead ) );
		}

		if ( RaytracingScene* raytracingScene = scene.GetRaytracingScene() )
		{
			raytracingScene->Build( renderGraph );
		}

		CalcVisibility( renderViewGroup );

		for ( RenderViewInfo& viewInfo : m_viewInfo )
		{
			for ( const PrimitiveSceneInfo* primitiveSceneInfo : renderScene.Primitives() )
			{
				primitiveSceneInfo->Proxy()->GatherDynamicMeshDrawInfo( viewInfo );
			}
		}
	}

	void SceneRenderer::InitDynamicShadows( RenderViewGroup& renderViewGroup )
	{
		IScene& scene = renderViewGroup.Scene();
		if ( scene.GetRenderScene() == nullptr )
		{
			return;
		}

		RenderFrameArray<ShadowInfo*> viewDependentShadow;
		RenderFrameArray<ShadowInfo*> viewIndependentShadow;

		Scene& renderScene = *scene.GetRenderScene();
		const auto& lights = renderScene.Lights();
		m_shadowInfos.reserve( lights.Size() );

		for ( const auto& view : m_viewInfo )
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
		ClassifyViewIndependentShadowCasterAndReceiver( viewIndependentShadow );

		SetupShadow();

		AllocateShadowMaps();
	}

	void SceneRenderer::ClassifyViewDependentShadowCasterAndReceiver( IScene& scene, const RenderFrameArray<ShadowInfo*>& shadows )
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
				if ( intersectionInfo.m_castShadow == false )
				{
					continue;
				}

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

	void SceneRenderer::ClassifyViewIndependentShadowCasterAndReceiver( const RenderFrameArray<ShadowInfo*>& shadows )
	{
		for ( ShadowInfo* pShadowInfo : shadows )
		{
			LightSceneInfo* lightSceneInfo = pShadowInfo->GetLightSceneInfo();

			const auto& intersectionInfos = lightSceneInfo->Primitives();
			for ( const auto& intersectionInfo : intersectionInfos )
			{
				if ( intersectionInfo.m_castShadow == false )
				{
					continue;
				}

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
		RenderFrameArray<ShadowInfo*> cascadeShadows;
		RenderFrameArray<ShadowInfo*> pointShadows;

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

	void SceneRenderer::AllocateCascadeShadowMaps( const RenderFrameArray<ShadowInfo*>& shadows )
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
				.m_access = agl::ResourceAccess::Default,
				.m_bindType = agl::ResourceBindType::RenderTarget | agl::ResourceBindType::ShaderResource,
				.m_miscFlag = agl::ResourceMisc::None,
				.m_clearValue = agl::ResourceClearValue{
					.m_color = { 1.f, 1.f, 1.f, 1.f }
				}
			};

			shadow->ShadowMap().m_shadowMaps.emplace_back( GraphicsResourcePool::GetInstance().FindFreeTexture( trait, "Shadow.Cascade" ) );

			agl::TextureTrait depthTrait = {
				.m_width = width,
				.m_height = height,
				.m_depth = CascadeShadowSetting::MAX_CASCADE_NUM, // Cascade map count, Right now, it's fixed constant.
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

			shadow->ShadowMap().m_shadowMapDepth = GraphicsResourcePool::GetInstance().FindFreeTexture( depthTrait, "Shadow.Cascade.Depth" );

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
					.m_access = agl::ResourceAccess::Default,
					.m_bindType = agl::ResourceBindType::RenderTarget | agl::ResourceBindType::ShaderResource,
					.m_miscFlag = agl::ResourceMisc::None,
					.m_clearValue = agl::ResourceClearValue{
						.m_color = { 0.f, 0.f, 0.f, 1.f }
					}
				};

				shadow->ShadowMap().m_shadowMaps.emplace_back( GraphicsResourcePool::GetInstance().FindFreeTexture( positionMapTrait, "RSMs.Position" ) );

				agl::TextureTrait normalMapTrait = {
					.m_width = width,
					.m_height = height,
					.m_depth = CascadeShadowSetting::MAX_CASCADE_NUM, // Cascade map count, Right now, it's fixed constant.
					.m_sampleCount = 1,
					.m_sampleQuality = 0,
					.m_mipLevels = 1,
					.m_format = agl::ResourceFormat::R10G10B10A2_UNORM,
					.m_access = agl::ResourceAccess::Default,
					.m_bindType = agl::ResourceBindType::RenderTarget | agl::ResourceBindType::ShaderResource,
					.m_miscFlag = agl::ResourceMisc::None,
					.m_clearValue = agl::ResourceClearValue{
						.m_color = { 0.f, 0.f, 0.f, 1.f }
					}
				};

				shadow->ShadowMap().m_shadowMaps.emplace_back( GraphicsResourcePool::GetInstance().FindFreeTexture( normalMapTrait, "RSMs.Normal" ) );

				agl::TextureTrait fluxMapTrait = {
					.m_width = width,
					.m_height = height,
					.m_depth = CascadeShadowSetting::MAX_CASCADE_NUM, // Cascade map count, Right now, it's fixed constant.
					.m_sampleCount = 1,
					.m_sampleQuality = 0,
					.m_mipLevels = 1,
					.m_format = agl::ResourceFormat::R8G8B8A8_UNORM,
					.m_access = agl::ResourceAccess::Default,
					.m_bindType = agl::ResourceBindType::RenderTarget | agl::ResourceBindType::ShaderResource,
					.m_miscFlag = agl::ResourceMisc::None,
					.m_clearValue = agl::ResourceClearValue{
						.m_color = { 0.f, 0.f, 0.f, 1.f }
					}
				};

				shadow->ShadowMap().m_shadowMaps.emplace_back( GraphicsResourcePool::GetInstance().FindFreeTexture( fluxMapTrait, "RSMs.Flux" ) );
			}
		}
	}

	void SceneRenderer::AllocatePointShadowMaps( const RenderFrameArray<ShadowInfo*>& shadows )
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
				.m_access = agl::ResourceAccess::Default,
				.m_bindType = agl::ResourceBindType::RenderTarget | agl::ResourceBindType::ShaderResource,
				.m_miscFlag = agl::ResourceMisc::TextureCube,
				.m_clearValue = agl::ResourceClearValue{
					.m_color = { 1.f, 1.f, 1.f, 1.f }
				}
			};

			shadow->ShadowMap().m_shadowMaps.emplace_back( GraphicsResourcePool::GetInstance().FindFreeTexture( trait, "Shadow.Point" ) );

			agl::TextureTrait depthTrait = {
				.m_width = width,
				.m_height = height,
				.m_depth = 6,
				.m_sampleCount = 1,
				.m_sampleQuality = 0,
				.m_mipLevels = 1,
				.m_format = agl::ResourceFormat::D24_UNORM_S8_UINT,
				.m_access = agl::ResourceAccess::Default,
				.m_bindType = agl::ResourceBindType::DepthStencil,
				.m_miscFlag = agl::ResourceMisc::TextureCube,
				.m_clearValue = agl::ResourceClearValue{
					.m_depthStencil = {
						.m_depth = 1.f,
						.m_stencil = 0
					}
				}
			};

			shadow->ShadowMap().m_shadowMapDepth = GraphicsResourcePool::GetInstance().FindFreeTexture( depthTrait, "Shadow.Point.Depth" );
		}
	}

	RenderFrameArray<VisibleDrawSnapshot>* SceneRenderer::GatherSortedDrawSnapshots( IScene& scene, RenderPassType passType, uint32 viewIndex )
	{
		const auto& primitives = scene.Primitives();
		if ( primitives.Size() == 0 )
		{
			return nullptr;
		}

		auto* renderScene = scene.GetRenderScene();
		if ( renderScene == nullptr )
		{
			return nullptr;
		}

		RenderViewInfo& viewInfo = m_viewInfo[viewIndex];
		auto& snapshots = viewInfo.m_drawSnapshots[static_cast<uint32>( passType )];

		// Create DrawSnapshot
		for ( auto primitive : primitives )
		{
			uint32 primitiveId = primitive->PrimitiveId();
			if ( viewInfo.m_visibilityMap[primitiveId] == false )
			{
				continue;
			}

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
						DrawSnapshot& snapshot = primitive->GetCachedDrawSnapshot( *snapshotIndex );

						VisibleDrawSnapshot& visibleSnapshot = snapshots.emplace_back();
						visibleSnapshot.m_primitiveId = primitiveId;
						visibleSnapshot.m_numInstance = 1;
						visibleSnapshot.m_snapshotBucketId = info.m_snapshotBucketId;
						visibleSnapshot.m_drawSnapshot = &snapshot;
					}
				}
			}
			else
			{
				proxy->TakeSnapshot( m_curFrameDrawSnapshots, snapshots );
			}
		}

		SortDrawSnapshots( snapshots );

		return snapshots.empty() ? nullptr : &snapshots;
	}

	VisibilityPassData SceneRenderer::BuildVisibilityPassData( IScene& scene, uint32 viewIndex )
	{
		const auto& primitives = scene.Primitives();
		if ( primitives.Size() == 0 )
		{
			return {};
		}

		auto* renderScene = scene.GetRenderScene();
		if ( renderScene == nullptr )
		{
			return {};
		}

		CPU_PROFILE( SceneRenderer_BuildVisibilityPassData );

		RenderViewInfo& viewInfo = m_viewInfo[viewIndex];

		struct VisibilitySortEntry
		{
			DrawSnapshot* m_drawSnapshot = nullptr;
			int32 m_drawSnapshotBucketId = -1;

			ShadingSnapshot* m_shadingSnapshot = nullptr;
			uint32 m_uniqueShadingSnapshotId = 0;

			uint32 m_primitiveId = 0;
		};

		RenderFrameArray<VisibilitySortEntry> visibilitySortEntries;

		for ( auto primitive : primitives )
		{
			uint32 primitiveId = primitive->PrimitiveId();
			if ( viewInfo.m_visibilityMap[primitiveId] == false )
			{
				continue;
			}

			PrimitiveProxy* proxy = primitive->Proxy();

			const std::vector<PrimitiveSubMeshInfo>& subMeshInfos = primitive->SubMeshInfos();

			if ( subMeshInfos.size() > 0 )
			{
				for ( const auto& subMeshInfo : subMeshInfos )
				{
					VisibilitySortEntry visibilitySortEntry;
					visibilitySortEntry.m_primitiveId = proxy->PrimitiveId();

					auto drawSnapshotIndex = subMeshInfo.GetCachedDrawSnapshotInfoIndex( RenderPassType::Visibility );
					if ( drawSnapshotIndex )
					{
						const CachedDrawSnapshotInfo& info = primitive->GetCachedDrawSnapshotInfo( *drawSnapshotIndex );
						DrawSnapshot& snapshot = primitive->GetCachedDrawSnapshot( *drawSnapshotIndex );

						visibilitySortEntry.m_drawSnapshot = &snapshot;
						visibilitySortEntry.m_drawSnapshotBucketId = info.m_snapshotBucketId;
					}
					else
					{
						// TODO
					}

					auto shadingSnapshotId = subMeshInfo.GetShadingSnapshotId();
					if ( shadingSnapshotId > -1 )
					{
						ShadingSnapshot& snapshot = primitive->GetShadingSnapshot( shadingSnapshotId );

						visibilitySortEntry.m_shadingSnapshot = &snapshot;
					}
					else
					{
						// TODO
					}

					if ( visibilitySortEntry.m_drawSnapshot != nullptr && visibilitySortEntry.m_shadingSnapshot != nullptr )
					{
						visibilitySortEntries.emplace_back( visibilitySortEntry );
					}
				}
			}
		}

		int32 lastShadingSnapshotId = 1;
		RenderFrameMap<VisibleShadingSnapshot, int32> uniqueShadingSnapshotMap;
		for ( VisibilitySortEntry& visibilitySortEntry : visibilitySortEntries )
		{
			VisibleShadingSnapshot visibleSnapshot = {
				.m_primitiveId = visibilitySortEntry.m_primitiveId,
				.m_shadingSnapshot = visibilitySortEntry.m_shadingSnapshot,
			};

			auto result = uniqueShadingSnapshotMap.emplace( visibleSnapshot, lastShadingSnapshotId );
			if ( result.second )
			{
				++lastShadingSnapshotId;
			}

			visibilitySortEntry.m_uniqueShadingSnapshotId = result.first->second;
		}

		std::ranges::sort( visibilitySortEntries,
		                   []( const VisibilitySortEntry& lhs, const VisibilitySortEntry& rhs )
		                   {
			                   return lhs.m_drawSnapshotBucketId < rhs.m_drawSnapshotBucketId;
		                   } );

		VisibilityPassData visibilityPassData;
		visibilityPassData.m_maxShadingSnapshotId = lastShadingSnapshotId;

		visibilityPassData.m_visibilityPassSnapshots.reserve( visibilitySortEntries.size() );
		visibilityPassData.m_shadingSnapshotIds.reserve( visibilitySortEntries.size() );
		visibilityPassData.m_shadingSnapshots.resize( uniqueShadingSnapshotMap.size() );

		for ( size_t i = 0; i < visibilitySortEntries.size(); ++i )
		{
			VisibilitySortEntry& visibilitySortEntry = visibilitySortEntries[i];

			VisibleDrawSnapshot& visibleSnapshot = visibilityPassData.m_visibilityPassSnapshots.emplace_back();
			visibleSnapshot.m_primitiveId = visibilitySortEntry.m_primitiveId;
			visibleSnapshot.m_primitiveIdOffset = static_cast<uint32>( i );
			visibleSnapshot.m_numInstance = 1;
			visibleSnapshot.m_snapshotBucketId = visibilitySortEntry.m_drawSnapshotBucketId;
			visibleSnapshot.m_drawSnapshot = visibilitySortEntry.m_drawSnapshot;

			visibilityPassData.m_shadingSnapshotIds.emplace_back( visibilitySortEntry.m_uniqueShadingSnapshotId );
		}

		for ( auto& [visibleShadingSnapshot, id] : uniqueShadingSnapshotMap )
		{
			visibilityPassData.m_shadingSnapshots[id - 1] = visibleShadingSnapshot;
		}

		MergeDrawSnapshots( visibilityPassData.m_visibilityPassSnapshots );

		return visibilityPassData;
	}

	void SceneRenderer::RenderShadowDepthPass( RenderGraph& renderGraph )
	{
		for ( ShadowInfo& shadowInfo : m_shadowInfos )
		{
			ShadowMapRenderTarget& shadowMap = shadowInfo.ShadowMap();
			assert( ( shadowMap.m_shadowMaps.size() > 0 )
				&& ( shadowMap.m_shadowMaps[0] != nullptr )
				&& ( shadowMap.m_shadowMapDepth != nullptr ) );

			auto [width, height] = shadowInfo.ShadowMapSize();

			RasterOutput rasterOutput;
			for ( int32 i = 0; i < shadowMap.m_shadowMaps.size(); ++i )
			{
				auto rgShadowMap = renderGraph.RegisterExternalResource( shadowMap.m_shadowMaps[i].Get() );
				rasterOutput.SetRenderTarget( i, rgShadowMap, RasterOutputLoadAction::Clear );
			}
			auto rgShadowMapDepth = renderGraph.RegisterExternalResource( shadowMap.m_shadowMapDepth.Get() );
			rasterOutput.SetDepthStencil( rgShadowMapDepth, false, RasterOutputLoadAction::Clear );
			rasterOutput.SetViewport( width, height );
			rasterOutput.SetScissorRect( width, height );

			GPU_PROFILE_EVENT( renderGraph, ShadowDepth );

			renderGraph.AddPass(
				rasterOutput,
				[this, &shadowInfo]( CommandList& commandList )
				{
					shadowInfo.SetupShadowConstantBuffer();
					shadowInfo.RenderDepth( commandList, m_resourceBinder );
				} );

			if ( DefaultRenderCore::IsESMsEnabled() )
			{
				shadowMap.m_shadowMaps[0] = GenerateExponentialShadowMaps( renderGraph, shadowInfo, shadowMap.m_shadowMaps[0] );
			}
		}
	}

	void SceneRenderer::RenderTexturedSky( RenderGraph& renderGraph, IScene& scene, const RasterOutput& rasterOutput )
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

		GPU_PROFILE_EVENT( renderGraph, TexturedSky );

		renderGraph.AddPass(
			rasterOutput,
			[this, proxy]( CommandList& commandList )
			{
				MaterialResource* material = proxy->GetMaterialResource();
				ShaderStates shaderState = {
					.m_vertexShader = material->GetVertexShader(),
					.m_pixelShader = material->GetPixelShader(),
				};

				StaticMeshRenderData* renderData = proxy->GetRenderData();
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

					m_resourceBinder.Bind( pipelineState.m_shaderState, snapshot.m_shaderBindings );

					AddSingleDrawPass( commandList, snapshot );
				}
			} );
	}

	void SceneRenderer::RenderShadow( RenderGraph& renderGraph, RenderViewGroup& renderViewGroup )
	{
		CPU_PROFILE( SceneRenderer_RenderShadow );

		auto renderTarget = GetRenderTargets().GetSceneColor();
		auto depthStencil = GetRenderTargets().GetDepthStencil();

		auto rgRenderTarget = renderGraph.RegisterExternalResource( renderTarget );
		auto rgDepthStencil = renderGraph.RegisterExternalResource( depthStencil );

		auto [width, height] = renderViewGroup.GetViewport().Size();

		RasterOutput rasterOutput;
		rasterOutput.SetRenderTarget( 0, rgRenderTarget );
		rasterOutput.SetDepthStencil( rgDepthStencil, true );
		rasterOutput.SetViewport( width, height );
		rasterOutput.SetScissorRect( width, height );

		auto viewSpaceDistance = GetRenderTargets().GetViewSpaceDistance();
		auto worldNormal = GetRenderTargets().GetWorldNormal();

		auto rgViewSpaceDistance = renderGraph.RegisterExternalResource( viewSpaceDistance );
		auto rgWorldNormal = renderGraph.RegisterExternalResource( worldNormal );

		GPU_PROFILE_EVENT( renderGraph, Shadow );

		for ( ShadowInfo& shadowInfo : m_shadowInfos )
		{
			BEGIN_RG_RESOURCE_STRUCT( RenderShadowPassResource )
				DECLARE_RG_TEXTURE_PIXEL_SRV( shadowMap )
				DECLARE_RG_TEXTURE_PIXEL_SRV( viewSpaceDistance )
				DECLARE_RG_TEXTURE_PIXEL_SRV( worldNormal )
			END_RG_RESOURCE_STRUCT();

			auto rgShadowMap = renderGraph.RegisterExternalResource( shadowInfo.ShadowMap().m_shadowMaps[0].Get() );

			RenderShadowPassResource passResource = {
				.m_shadowMap = rgShadowMap,
				.m_viewSpaceDistance = rgViewSpaceDistance,
				.m_worldNormal = rgWorldNormal,
			};

			renderGraph.AddPass(
				passResource,
				rasterOutput,
				[this, passResource, &shadowInfo](CommandList& commandList)
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
					m_resourceBinder.Bind( pipelineState.m_shaderState, snapshot.m_shaderBindings );

					m_resourceBinder.Add( StaticName( "ShadowTexture" ), passResource.m_shadowMap->SRV() );

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

					m_resourceBinder.Add( StaticName( "ShadowSampler" ), shadowSampler.Resource() );

					m_resourceBinder.Add( StaticName( "ShadowDepthPassParameters" ), shadowInfo.GetShadowShaderArguments().Resource() );

					if ( bESMsEnabled )
					{
						m_resourceBinder.Add( StaticName( "ESMsParameters" ), shadowInfo.GetESMsShaderArguments().Resource() );
					}

					m_resourceBinder.Bind( pipelineState.m_shaderState, snapshot.m_shaderBindings );

					AddSingleDrawPass( commandList, snapshot );
				}
			);
		}
	}

	void SceneRenderer::RenderSkyAtmosphere( RenderGraph& renderGraph, RenderViewGroup& renderViewGroup, uint32 viewIndex )
	{
		CPU_PROFILE( SceneRenderer_RenderSkyAtmosphere );

		Scene* renderScene = renderViewGroup.Scene().GetRenderScene();
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

		auto rgTransmittanceLut = renderGraph.RegisterExternalResource( info->GetTransmittanceLutTexture().Get() );
		auto rgIrradianceLut = renderGraph.RegisterExternalResource( info->GetIrradianceLutTexture().Get() );
		auto rgInscatterLut = renderGraph.RegisterExternalResource( info->GetInscatterLutTexture().Get() );

		BEGIN_RG_RESOURCE_STRUCT( RenderSkyAtmospherePassResource )
			DECLARE_RG_TEXTURE_PIXEL_SRV( transmittanceLut )
			DECLARE_RG_TEXTURE_PIXEL_SRV( irradianceLut )
			DECLARE_RG_TEXTURE_PIXEL_SRV( inscatterLut )
		END_RG_RESOURCE_STRUCT();

		RenderSkyAtmospherePassResource passResource = {
			.m_transmittanceLut = rgTransmittanceLut,
			.m_irradianceLut = rgIrradianceLut,
			.m_inscatterLut = rgInscatterLut,
		};

		auto renderTarget = GetRenderTargets().GetSceneColor();
		auto depthStencil = GetRenderTargets().GetDepthStencil();

		auto rgRenderTarget = renderGraph.RegisterExternalResource( renderTarget );
		auto rgDepthStencil = renderGraph.RegisterExternalResource( depthStencil );

		auto [width, height] = renderViewGroup.GetViewport().Size();

		RasterOutput rasterOutput;
		rasterOutput.SetRenderTarget( 0, rgRenderTarget );
		rasterOutput.SetDepthStencil( rgDepthStencil, true );
		rasterOutput.SetViewport( width, height );
		rasterOutput.SetScissorRect( width, height );

		GPU_PROFILE_EVENT( renderGraph, SkyAtmosphere );

		renderGraph.AddPass(
			passResource,
			rasterOutput,
			[this, info, skyAtmosphereLight, viewIndex]( CommandList& commandList )
			{
				LightProperty lightProperty = skyAtmosphereLight->Proxy()->GetLightProperty();

				auto& skyAtmosphereRenderParameter = info->GetShaderArguments();
				SkyAtmosphereRenderParameters params = {
					.CameraPos = m_viewInfo[viewIndex].m_viewOrigin,
					.SunDir = -lightProperty.m_direction,
					.Exposure = 0.4f,
				};
				skyAtmosphereRenderParameter->Update( params );

				SkyAtmosphereDrawPassProcessor skyAtmosphereDrawPassProcessor;
				auto result = skyAtmosphereDrawPassProcessor.Process( FullScreenQuadDrawInfo() );
				if ( result.has_value() == false )
				{
					return;
				}

				DrawSnapshot& snapshot = *result;

				// Update invalidated resources
				GraphicsPipelineState& pipelineState = snapshot.m_pipelineState;
				m_resourceBinder.Bind( pipelineState.m_shaderState, snapshot.m_shaderBindings );
				
				auto linearSampler = StaticSamplerState<>::Get();

				ResourceBinder passResourceBinder;
				passResourceBinder.Add( StaticName( "TransmittanceLut" ), info->GetTransmittanceLutTexture()->SRV() );
				passResourceBinder.Add( StaticName( "TransmittanceLutSampler" ), linearSampler.Resource() );
				passResourceBinder.Add( StaticName( "IrradianceLut" ), info->GetIrradianceLutTexture()->SRV() );
				passResourceBinder.Add( StaticName( "IrradianceLutSampler" ), linearSampler.Resource() );
				passResourceBinder.Add( StaticName( "InscatterLut" ), info->GetInscatterLutTexture()->SRV() );
				passResourceBinder.Add( StaticName( "InscatterLutSampler" ), linearSampler.Resource() );
				passResourceBinder.Add( StaticName( "SkyAtmosphereRenderParameter" ), skyAtmosphereRenderParameter->Resource() );

				passResourceBinder.Bind( pipelineState.m_shaderState, snapshot.m_shaderBindings );

				AddSingleDrawPass( commandList, snapshot );
			}
		);
	}

	void SceneRenderer::RenderVolumetricCloud( RenderGraph& renderGraph, RenderViewGroup& renderViewGroup )
	{
		CPU_PROFILE( SceneRenderer_RenderVolumetricCloud );

		Scene* renderScene = renderViewGroup.Scene().GetRenderScene();
		if ( renderScene == nullptr )
		{
			return;
		}

		VolumetricCloudSceneInfo* info = renderScene->VolumetricCloud();
		if ( info == nullptr )
		{
			return;
		}

		const LightSceneInfo* skyAtmosphereLight = renderScene->SkyAtmosphereSunLight();
		if ( skyAtmosphereLight == nullptr )
		{
			return;
		}

		auto rgBaseCloudShape = renderGraph.RegisterExternalResource( info->BaseCloudShape() );
		auto rgDetailCloudShape = renderGraph.RegisterExternalResource( info->DetailCloudShape() );
		auto rgWeatherMap = renderGraph.RegisterExternalResource( info->WeatherMap() );

		BEGIN_RG_RESOURCE_STRUCT( RenderVolumetricCloudPassResource )
			DECLARE_RG_TEXTURE_PIXEL_SRV( baseCloudShape )
			DECLARE_RG_TEXTURE_PIXEL_SRV( detailCloudShape )
			DECLARE_RG_TEXTURE_PIXEL_SRV( weatherMap )
		END_RG_RESOURCE_STRUCT();

		RenderVolumetricCloudPassResource passResource = {
			.m_baseCloudShape = rgBaseCloudShape,
			.m_detailCloudShape = rgDetailCloudShape,
			.m_weatherMap = rgWeatherMap,
		};

		auto renderTarget = GetRenderTargets().GetSceneColor();
		auto depthStencil = GetRenderTargets().GetDepthStencil();

		auto rgRenderTarget = renderGraph.RegisterExternalResource( renderTarget );
		auto rgDepthStencil = renderGraph.RegisterExternalResource( depthStencil );

		auto [width, height] = renderViewGroup.GetViewport().Size();

		RasterOutput rasterOutput;
		rasterOutput.SetRenderTarget( 0, rgRenderTarget );
		rasterOutput.SetDepthStencil( rgDepthStencil, true );
		rasterOutput.SetViewport( width, height );
		rasterOutput.SetScissorRect( width, height );

		GPU_PROFILE_EVENT( renderGraph, VolumetricCloud );

		renderGraph.AddPass(
			passResource,
			rasterOutput,
			[this, info, skyAtmosphereLight]( CommandList& commandList )
			{
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
				VolumetricCloudRenderParameters params = {
					.SphereRadius = Vector( proxy.EarthRadius(), proxy.InnerRadius(), proxy.OuterRadius() ),
					.LightAbsorption = proxy.LightAbsorption(),
					.LightPosOrDir = lightPosOrDir,
					.CloudColor = proxy.CloudColor(),
					.WindDirection = Vector( 0.5f, 0.f, 0.1f ).GetNormalized(),
					.WindSpeed = 450.f,
					.Crispiness = proxy.Crispiness(),
					.Curliness = proxy.Curliness(),
					.DensityFactor = proxy.DensityFactor(),
					.DensityScale = proxy.DensityScale(),
				};

				auto& shaderArguments = info->GetShaderArguments();
				shaderArguments.Update( params );

				DrawSnapshot& snapshot = *result;

				// Update invalidated resources
				GraphicsPipelineState& pipelineState = snapshot.m_pipelineState;
				m_resourceBinder.Bind( pipelineState.m_shaderState, snapshot.m_shaderBindings );

				SamplerState wrapSamplerState = StaticSamplerState<agl::TextureFilter::MinMagMipLinear
					, agl::TextureAddressMode::Wrap
					, agl::TextureAddressMode::Wrap
					, agl::TextureAddressMode::Wrap>::Get();

				ResourceBinder passResourceBinder;
				passResourceBinder.Add( StaticName( "VolumetricCloudRenderParameter" ), shaderArguments.Resource() );
				passResourceBinder.Add( StaticName( "BaseCloudShape" ), info->BaseCloudShape()->SRV() );
				passResourceBinder.Add( StaticName( "DetailCloudShape" ), info->DetailCloudShape()->SRV() );
				passResourceBinder.Add( StaticName( "WeatherMap" ), info->WeatherMap()->SRV() );
				passResourceBinder.Add( StaticName( "BaseCloudSampler" ), wrapSamplerState.Resource() );
				passResourceBinder.Add( StaticName( "WeatherSampler" ), wrapSamplerState.Resource() );

				passResourceBinder.Bind( pipelineState.m_shaderState, snapshot.m_shaderBindings );

				AddSingleDrawPass( commandList, snapshot );
			} );
	}

	void SceneRenderer::RenderVolumetricFog( RenderGraph& renderGraph, RenderViewGroup& renderViewGroup )
	{
		CPU_PROFILE( SceneRenderer_RenderVolumetricFog );

		Scene* renderScene = renderViewGroup.Scene().GetRenderScene();
		if ( renderScene == nullptr )
		{
			return;
		}

		VolumetricFogSceneInfo* info = renderScene->VolumetricFog();
		if ( info == nullptr )
		{
			return;
		}

		GPU_PROFILE_EVENT( renderGraph, VolumetricFog );

		info->PrepareFrustumVolume( renderGraph, *renderScene, m_forwardLighting, m_shadowInfos );

		auto rgAccumulatedVolume = renderGraph.RegisterExternalResource( info->AccumulatedVolume().Get() );

		BEGIN_RG_RESOURCE_STRUCT( RenderVolumetricFogPassResource )
			DECLARE_RG_TEXTURE_PIXEL_SRV( accumulatedVolume )
		END_RG_RESOURCE_STRUCT();

		RenderVolumetricFogPassResource renderVolumetricFogPassResource = {
			.m_accumulatedVolume = rgAccumulatedVolume
		};

		auto renderTarget = GetRenderTargets().GetSceneColor();
		auto depthStencil = GetRenderTargets().GetDepthStencil();

		auto rgRenderTarget = renderGraph.RegisterExternalResource( renderTarget );
		auto rgDepthStencil = renderGraph.RegisterExternalResource( depthStencil );

		auto [width, height] = renderViewGroup.GetViewport().Size();

		RasterOutput rasterOutput;
		rasterOutput.SetRenderTarget( 0, rgRenderTarget );
		rasterOutput.SetDepthStencil( rgDepthStencil, true );
		rasterOutput.SetViewport( width, height );
		rasterOutput.SetScissorRect( width, height );

		renderGraph.AddPass(
			renderVolumetricFogPassResource,
			rasterOutput,
			[this, info]( CommandList& commandList )
			{
				VolumetricFogDrawPassProcessor volumetricFogDrawPassProcessor;
				auto result = volumetricFogDrawPassProcessor.Process( FullScreenQuadDrawInfo() );
				if ( result.has_value() == false )
				{
					return;
				}

				DrawSnapshot& snapshot = *result;

				// Update invalidated resources
				GraphicsPipelineState& pipelineState = snapshot.m_pipelineState;
				m_resourceBinder.Bind( pipelineState.m_shaderState, snapshot.m_shaderBindings );

				SamplerState accumulatedVolumeSampler = StaticSamplerState<>::Get();

				ResourceBinder passResourceBinder;
				passResourceBinder.Add( StaticName( "AccumulatedVolume" ), info->AccumulatedVolume()->SRV() );
				passResourceBinder.Add( StaticName( "AccumulatedVolumeSampler" ), accumulatedVolumeSampler.Resource() );
				passResourceBinder.Add( StaticName( "VolumetricFogParameterBuffer" ), info->GetShaderArguments().Resource() );

				passResourceBinder.Bind( pipelineState.m_shaderState, snapshot.m_shaderBindings );

				AddSingleDrawPass( commandList, snapshot );
			} );
	}

	void SceneRenderer::RenderTemporalAntiAliasing( RenderGraph& renderGraph, RenderViewGroup& renderViewGroup )
	{
		GPU_PROFILE_EVENT( renderGraph, TAA );
		m_taaPass.Render( renderGraph, GetRenderTargets(), renderViewGroup );
	}

	void SceneRenderer::RenderIndirectIllumination( RenderGraph& renderGraph, RenderViewGroup& renderViewGroup, [[maybe_unused]] uint32 viewIndex )
	{
		CPU_PROFILE( SceneRenderer_RenderIndirectIllumination );

		GPU_PROFILE_EVENT( renderGraph, IndirectIllumination );

		if ( DefaultRenderCore::IsLpvEnabled() )
		{
			m_lpv.Prepare( renderGraph, renderViewGroup );
			m_lpv.InjectLight( renderGraph, renderViewGroup.Scene(), m_shadowInfos );
			m_lpv.Propagate( renderGraph );

			LpvRenderingParameters renderingParams = {
				.m_viewSpaceDistance = GetRenderTargets().GetViewSpaceDistance(),
				.m_worldNormal = GetRenderTargets().GetWorldNormal(),
			};

			m_resourceCollection.m_indirectIllumination = m_lpv.Render( renderGraph, renderingParams, m_resourceBinder );
		}
		else if ( DefaultRenderCore::IsRSMsEnabled() )
		{
			RSMsRenderingParam renderingParam = {
				.m_viewSpaceDistance = GetRenderTargets().GetViewSpaceDistance(),
				.m_worldNormal = GetRenderTargets().GetWorldNormal(),
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

			m_rsmPass.PreRender( renderViewGroup );
			m_resourceCollection.m_indirectIllumination = m_rsmPass.Render( renderGraph, renderingParam, m_resourceBinder );
		}
	}

	void SceneRenderer::RenderScreenSpaceIndirectIllumination( RenderGraph& renderGraph, RenderViewGroup& renderViewGroup, [[maybe_unused]] uint32 viewIndex )
	{
		if ( DefaultRenderCore::IsSSGIEnabled() == false )
		{
			return;
		}

		SSGIConfig ssgiConfig = DefaultRenderCore::GetSSGIConfig();
		
		SSGIRenderParams params = {
			.m_viewShaderArguments = renderViewGroup.Scene().GetViewShaderArguments().Resource(),
			.m_sceneColor = GetRenderTargets().GetSceneColor(),
			.m_viewSpaceDistance = GetRenderTargets().GetViewSpaceDistance(),
			.m_prevViewSpaceDistance = GetRenderTargets().GetPrevViewSpaceDistance(),
			.m_worldNormal = GetRenderTargets().GetWorldNormal(),
			.m_velocity = GetRenderTargets().GetVelocity(),
			.m_thickness = ssgiConfig.m_thickness,
			.m_viewSpaceRadius = ssgiConfig.m_viewSpaceRadius,
			.m_numSlices = ssgiConfig.m_numSlices,
			.m_numSteps = ssgiConfig.m_numSteps,
			.m_colorIntensity = ssgiConfig.m_colorIntensity,
			.m_denoiseKernelRadius = ssgiConfig.m_denoiseKernelRadius,
		};

		m_resourceCollection.m_ssgi = m_ssgiPass.Render( renderGraph, params );
	}

	void SceneRenderer::RenderDebugOverlay( RenderGraph& renderGraph, RenderViewGroup& renderViewGroup, uint32 viewIndex )
	{
		auto renderTarget = GetRenderTargets().GetSceneColor();
		auto depthStencil = GetRenderTargets().GetDepthStencil();

		auto rgRenderTarget = renderGraph.RegisterExternalResource( renderTarget );
		auto rgDepthStencil = renderGraph.RegisterExternalResource( depthStencil );

		auto [width, height] = renderViewGroup.GetViewport().Size();

		RasterOutput rasterOutput;
		rasterOutput.SetRenderTarget( 0, rgRenderTarget );
		rasterOutput.SetDepthStencil( rgDepthStencil, true );
		rasterOutput.SetViewport( width, height );
		rasterOutput.SetScissorRect( width, height );

		m_viewInfo[viewIndex].m_debugOverlayData.Draw( renderGraph, m_dynamicVertexBuffer, m_resourceBinder, rasterOutput );
	}

	void SceneRenderer::DoRenderHitProxy( RenderGraph& renderGraph, RenderViewGroup& renderViewGroup )
	{
		IScene& scene = renderViewGroup.Scene();
		const auto& primitives = scene.Primitives();
		if ( primitives.Size() == 0 )
		{
			return;
		}

		Viewport& viewport = renderViewGroup.GetViewport();
		auto [width, height] = viewport.Size();

		HitProxyMap& hitProxyMap = viewport.GetHitPorxyMap();
		hitProxyMap.Init( width, height );

		auto renderTarget = viewport.GetHitPorxyMap().Texture();

		agl::TextureTrait trait = {
				.m_width = width,
				.m_height = height,
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

		auto depthStencil = GraphicsResourcePool::GetInstance().FindFreeTexture( trait, "HitProxy.DepthStencil" );

		auto rgRenderTarget = renderGraph.RegisterExternalResource( renderTarget );
		auto rgDepthStencil = renderGraph.RegisterExternalResource( depthStencil.Get() );

		RasterOutput rasterOutput;
		rasterOutput.SetRenderTarget( 0, rgRenderTarget, RasterOutputLoadAction::Clear );
		rasterOutput.SetDepthStencil( rgDepthStencil, false, RasterOutputLoadAction::Clear );
		rasterOutput.SetViewport( width, height );
		rasterOutput.SetScissorRect( width, height );

		auto& viewShaderArguments = scene.GetViewShaderArguments();

		for ( size_t viewIndex = 0; viewIndex < renderViewGroup.NumRenderView(); ++viewIndex )
		{
			SceneViewParameters viewParam = GetViewParameters( ( viewIndex < m_prevFrameContext.size() ) ? &m_prevFrameContext[viewIndex] : nullptr
				, renderViewGroup, viewIndex );

			viewShaderArguments.Update( viewParam );

			RenderViewInfo& viewInfo = m_viewInfo[viewIndex];
			auto& snapshots = viewInfo.m_drawSnapshots[static_cast<uint32>( RenderPassType::HitProxy )];

			// Create DrawSnapshot
			for ( auto primitive : primitives )
			{
				uint32 primitiveId = primitive->PrimitiveId();
				if ( viewInfo.m_visibilityMap[primitiveId] == false )
				{
					continue;
				}

				const std::vector<PrimitiveSubMeshInfo>& subMeshInfos = primitive->SubMeshInfos();

				if ( subMeshInfos.size() > 0 )
				{
					for ( const auto& subMeshInfo : subMeshInfos )
					{
						auto snapshotIndex = subMeshInfo.GetCachedDrawSnapshotInfoIndex( RenderPassType::HitProxy );
						if ( snapshotIndex )
						{
							const CachedDrawSnapshotInfo& info = primitive->GetCachedDrawSnapshotInfo( *snapshotIndex );
							DrawSnapshot& snapshot = primitive->GetCachedDrawSnapshot( *snapshotIndex );

							VisibleDrawSnapshot& visibleSnapshot = snapshots.emplace_back();
							visibleSnapshot.m_primitiveId = primitiveId;
							visibleSnapshot.m_numInstance = 1;
							visibleSnapshot.m_snapshotBucketId = info.m_snapshotBucketId;
							visibleSnapshot.m_drawSnapshot = &snapshot;
						}
					}
				}
				else
				{
					// TODO : Is this line really necessary?
				}
			}

			if ( snapshots.size() != 0 )
			{
				renderGraph.AddPass(
					rasterOutput,
					[this, &snapshots, &primitives]( CommandList& commandList )
					{
						// Update invalidated resources
						for ( size_t i = 0; i < snapshots.size(); )
						{
							DrawSnapshot& snapshot = *snapshots[i].m_drawSnapshot;
							GraphicsPipelineState& pipelineState = snapshot.m_pipelineState;

							m_resourceBinder.Bind( pipelineState.m_shaderState, snapshot.m_shaderBindings );

							i += snapshots[i].m_numInstance;
						}

						VertexBuffer primitiveIds = GetPrimitiveIdPool().Alloc( static_cast<uint32>( snapshots.size() * sizeof( uint32 ) ) );

						auto idBuffer = static_cast<uint32*>( primitiveIds.Lock() );
						if ( idBuffer )
						{
							for ( size_t i = 0; i < snapshots.size(); ++i )
							{
								snapshots[i].m_primitiveIdOffset = static_cast<uint32>( i );
								*idBuffer = snapshots[i].m_primitiveId;
								++idBuffer;
							}

							primitiveIds.Unlock();
						}

						for ( size_t i = 0; i < snapshots.size(); )
						{
							HitProxyId hitProxyId = primitives[snapshots[i].m_primitiveId]->GetHitProxyId();

							SetShaderValue( commandList, HitProxyIdShaderParam, hitProxyId.GetColor().ToColorF() );
							CommitDrawSnapshot( commandList, snapshots[i], primitiveIds );

							i += snapshots[i].m_numInstance;
						}
					} );
			}
		}

		BEGIN_RG_RESOURCE_STRUCT( CopyHitProxyPassResource )
			DECLARE_RG_TEXTURE_COPY_DEST( copyDest )
			DECLARE_RG_TEXTURE_COPY_SOURCE( copySource )
		END_RG_RESOURCE_STRUCT();

		auto rgCopyDest = renderGraph.RegisterExternalResource( viewport.GetHitPorxyMap().CpuTexture() );
		auto rgCopySource = renderGraph.RegisterExternalResource( viewport.GetHitPorxyMap().Texture() );

		CopyHitProxyPassResource copyHitProxyPassResource = {
			.m_copyDest = rgCopyDest,
			.m_copySource = rgCopySource
		};

		renderGraph.AddPass(
			copyHitProxyPassResource,
			[copyHitProxyPassResource]( [[maybe_unused]] CopyCommandList& commandList )
			{
				commandList.CopyResource( copyHitProxyPassResource.m_copyDest->Get(), copyHitProxyPassResource.m_copySource->Get(), false);
			} );
	}

	void SceneRenderer::ResolveSceneColor( RenderGraph& renderGraph, RenderViewGroup& renderViewGroup )
	{
		Viewport& viewport = renderViewGroup.GetViewport();
		auto rgCopyDest = renderGraph.RegisterExternalResource( viewport.Texture() );
		auto rgCopySource = renderGraph.RegisterExternalResource( GetRenderTargets().GetSceneColor() );

		BEGIN_RG_RESOURCE_STRUCT( CopySceneColorPassResource )
			DECLARE_RG_TEXTURE_COPY_DEST( copyDest )
			DECLARE_RG_TEXTURE_COPY_SOURCE( copySource )
		END_RG_RESOURCE_STRUCT();

		CopySceneColorPassResource copySceneColorPassResource = {
			.m_copyDest = rgCopyDest,
			.m_copySource = rgCopySource
		};

		renderGraph.AddPass(
			copySceneColorPassResource,
			[copySceneColorPassResource]( [[maybe_unused]] CopyCommandList& commandList )
			{
				commandList.CopyResource( copySceneColorPassResource.m_copyDest->Get(), copySceneColorPassResource.m_copySource->Get(), false);
			} );
	}

	void SceneRenderer::CalcVisibility( RenderViewGroup& renderViewGroup )
	{
		IScene& scene = renderViewGroup.Scene();
		const auto& primitives = scene.Primitives();

		for ( RenderViewInfo& viewInfo : m_viewInfo )
		{
			Frustum frustum( viewInfo.m_viewProjMatrix );

			for ( auto primitive : primitives )
			{
				uint32 primitiveId = primitive->PrimitiveId();
				BoxSphereBounds bounds = scene.PrimitiveBounds()[primitiveId];

				CollisionResult result = BoxAndFrustum( bounds.Origin() - bounds.HalfSize(), bounds.Origin() + bounds.HalfSize(), frustum );

				viewInfo.m_visibilityMap[primitiveId] = ( result != CollisionResult::Outside );
			}

			OcclusionCull( scene, m_dynamicVertexBuffer, viewInfo, m_occlusionRenderData );
		}
	}

	void SceneRenderer::ResetFrameData()
	{
		ResetTransientContainerScope resetShadowInfos( m_shadowInfos );
		ResetTransientContainerScope resetDrawSnapshotsByView( m_drawSnapshotsByView );
		ResetTransientContainerScope resetOcclusionRenderData( m_occlusionRenderData );
		ResetTransientContainerScope resetCurrentFrameDrawSnapshots( m_curFrameDrawSnapshots );
		ResetTransientContainerScope resetCurFrameShadingSnapshots( m_curFrameShadingSnapshots );

		GetTransientAllocator<ThreadType::RenderThread>().Purge();
	}

	void AddSingleDrawPass( CommandList& commandList, DrawSnapshot& snapshot )
	{
		VisibleDrawSnapshot visibleSnapshot = {
			.m_primitiveId = 0,
			.m_primitiveIdOffset = 0,
			.m_numInstance = 1,
			.m_snapshotBucketId = -1,
			.m_drawSnapshot = &snapshot,
		};

		VertexBuffer emptyPrimitiveId;
		CommitDrawSnapshot( commandList, visibleSnapshot, emptyPrimitiveId );
	}

	PrimitiveIdVertexBufferPool& GetPrimitiveIdPool()
	{
		static PrimitiveIdVertexBufferPool primitiveIdBufferPool;
		return primitiveIdBufferPool;
	}
}
