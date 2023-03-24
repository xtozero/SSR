#include "stdafx.h"
#include "Renderer/SceneRenderer.h"

#include "CommandList.h"
#include "Config/DefaultRenderCoreConfig.h"
#include "Math/TransformationMatrix.h"
#include "Mesh/StaticMeshResource.h"
#include "Material/MaterialResource.h"
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
#include "ShaderBindings.h"
#include "ShadowDrawPassProcessor.h"
#include "ShadowSetup.h"
#include "SkyAtmosphereRendering.h"
#include "TransitionUtils.h"
#include "Viewport.h"
#include "VolumetricCloudPassProcessor.h"

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

	void SceneRenderer::WaitUntilRenderingIsFinish()
	{
		PrimitiveIdVertexBufferPool::GetInstance().DiscardAll();
	}

	void SceneRenderer::InitDynamicShadows( RenderViewGroup& renderViewGroup )
	{
		IScene& scene = renderViewGroup.Scene();
		if ( scene.GetRenderScene() == nullptr )
		{
			return;
		}

		RenderThreadFrameData<ShadowInfo*> viewDependentShadow;

		Scene& renderScene = *scene.GetRenderScene();
		auto lights = renderScene.Lights();

		for ( const auto& view : renderViewGroup )
		{
			for ( LightSceneInfo* light : lights )
			{
				LightProxy* proxy = light->Proxy();
				if ( proxy->CastShadow() )
				{
					ShadowInfo& shadowInfo = m_shadowInfos.emplace_back( light, view );
					viewDependentShadow.push_back( &shadowInfo );
				}
			}
		}

		ClassifyShadowCasterAndReceiver( scene, viewDependentShadow );

		SetupShadow();

		AllocateShadowMaps();
	}

	void SceneRenderer::ClassifyShadowCasterAndReceiver( IScene& scene, const RenderThreadFrameData<ShadowInfo*>& shadows )
	{
		Scene& renderScene = *scene.GetRenderScene();

		for ( ShadowInfo* pShadowInfo : shadows )
		{
			LightSceneInfo* lightSceneInfo = pShadowInfo->GetLightSceneInfo();
			[[maybe_unused]] LightType lightType = lightSceneInfo->Proxy()->GetLightType();

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

	void SceneRenderer::SetupShadow()
	{
		for ( ShadowInfo& shadowInfo : m_shadowInfos )
		{
			if ( shadowInfo.HasCasterPrimitives() == false )
			{
				continue;
			}

			LightSceneInfo* lightSceneInfo = shadowInfo.GetLightSceneInfo();
			LightType lightType = lightSceneInfo->Proxy()->GetLightType();

			switch ( lightType )
			{
			case LightType::Directional:
			{
				BuildOrthoShadowProjectionMatrix( shadowInfo );
				break;
			}
			case LightType::Point:
				break;
			case LightType::Spot:
				break;
			case LightType::None:
			default:
				break;
			}
		}
	}

	void SceneRenderer::AllocateShadowMaps()
	{
		RenderThreadFrameData<ShadowInfo*> cascadeShadows;

		for ( auto& shadowInfo : m_shadowInfos )
		{
			LightSceneInfo* lightSceneInfo = shadowInfo.GetLightSceneInfo();
			LightType lightType = lightSceneInfo->Proxy()->GetLightType();

			switch ( lightType )
			{
			case LightType::None:
				break;
			case LightType::Directional:
				cascadeShadows.push_back( &shadowInfo );
				break;
			case LightType::Point:
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
				.m_access = agl::ResourceAccessFlag::GpuRead | agl::ResourceAccessFlag::GpuWrite,
				.m_bindType = agl::ResourceBindType::RenderTarget | agl::ResourceBindType::ShaderResource,
				.m_miscFlag = agl::ResourceMisc::None,
				.m_clearValue = agl::ResourceClearValue{
					.m_color = { 1.f, 1.f, 1.f, 1.f }
				}
			};

			shadow->ShadowMap().m_shadowMap = RenderTargetPool::GetInstance().FindFreeRenderTarget( trait );

			agl::TextureTrait depthTrait = { 
				.m_width = width,
				.m_height = height,
				.m_depth = CascadeShadowSetting::MAX_CASCADE_NUM, // Cascade map count, Right now, it's fixed constant.
				.m_sampleCount = 1,
				.m_sampleQuality = 0,
				.m_mipLevels = 1,
				.m_format = agl::ResourceFormat::D24_UNORM_S8_UINT,
				.m_access = agl::ResourceAccessFlag::GpuRead | agl::ResourceAccessFlag::GpuWrite,
				.m_bindType = agl::ResourceBindType::DepthStencil,
				.m_miscFlag = agl::ResourceMisc::None,
				.m_clearValue = agl::ResourceClearValue{
					.m_depthStencil = {
						.m_depth = 1.f,
						.m_stencil = 0
					}
				}
			};

			shadow->ShadowMap().m_shadowMapDepth = RenderTargetPool::GetInstance().FindFreeRenderTarget( depthTrait );

			EnqueueRenderTask(
				[texture = shadow->ShadowMap().m_shadowMap, depthTexture = shadow->ShadowMap().m_shadowMapDepth]()
				{
					if ( texture )
					{
						texture->Init();
					}

					if ( depthTexture )
					{
						depthTexture->Init();
					}
				} );
		}
	}

	void SceneRenderer::RenderShadowDepthPass()
	{
		for ( ShadowInfo& shadowInfo : m_shadowInfos )
		{
			ShadowMapRenderTarget& shadowMap = shadowInfo.ShadowMap();
			assert( ( shadowMap.m_shadowMap != nullptr ) && ( shadowMap.m_shadowMapDepth != nullptr ) );

			auto [width, height] = shadowInfo.ShadowMapSize();

			RenderingOutputContext context = {
				{ shadowMap.m_shadowMap },
				shadowMap.m_shadowMapDepth,
				{ 0.f, 0.f, static_cast<float>( width ), static_cast<float>( height ), 0.f, 1.f },
				{ 0L, 0L, static_cast<int32>( width ), static_cast<int32>( height ) }
			};
			StoreOuputContext( context );

			auto commandList = GetCommandList();

			agl::ResourceTransition transitions[] = {
				Transition( *shadowMap.m_shadowMap.Get(),agl::ResourceState::RenderTarget ),
				Transition( *shadowMap.m_shadowMapDepth.Get(), agl::ResourceState::DepthWrite )
			};

			commandList.Transition( std::extent_v<decltype( transitions )>, transitions);

			agl::RenderTargetView* rtv = shadowMap.m_shadowMap->RTV();
			commandList.ClearRenderTarget( rtv, { 1, 1, 1, 1 } );

			agl::DepthStencilView* dsv = shadowMap.m_shadowMapDepth->DSV();
			commandList.ClearDepthStencil( dsv, 1.f, 0 );

			shadowInfo.SetupShadowConstantBuffer();
			shadowInfo.RenderDepth( *this, m_shaderResources );
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

			VisibleDrawSnapshot visibleSnapshot = {
				0,
				0,
				1,
				-1,
				&snapshot,
			};

			VertexBuffer emptyPrimitiveID;
			CommitDrawSnapshot( commandList, visibleSnapshot, emptyPrimitiveID );
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

		VertexBuffer primitiveIds = PrimitiveIdVertexBufferPool::GetInstance().Alloc( static_cast<uint32>( snapshots.size() * sizeof( uint32 ) ) );

		SortDrawSnapshots( snapshots, primitiveIds );
		// CommitDrawSnapshots( *this, renderViewGroup, curView, primitiveIds );
		ParallelCommitDrawSnapshot( *this, snapshots, primitiveIds );
	}

	void SceneRenderer::RenderShadow()
	{
		for ( ShadowInfo& shadowInfo : m_shadowInfos )
		{
			ShadowDrawPassProcessor shadowDrawPassProcessor;

			PrimitiveSubMesh meshInfo;
			meshInfo.m_count = 3;

			auto result = shadowDrawPassProcessor.Process( meshInfo );
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

			m_shaderResources.AddResource( "ShadowTexture", shadowInfo.ShadowMap().m_shadowMap->SRV() );

			SamplerOption shadowSamplerOption;
			shadowSamplerOption.m_filter |= agl::TextureFilter::Comparison;
			shadowSamplerOption.m_addressU = agl::TextureAddressMode::Border;
			shadowSamplerOption.m_addressV = agl::TextureAddressMode::Border;
			shadowSamplerOption.m_addressW = agl::TextureAddressMode::Border;
			shadowSamplerOption.m_comparisonFunc = agl::ComparisonFunc::LessEqual;
			SamplerState shadowSampler = GraphicsInterface().FindOrCreate( shadowSamplerOption );
			m_shaderResources.AddResource( "ShadowSampler", shadowSampler.Resource() );

			m_shaderResources.AddResource( "ShadowDepthPassParameters", shadowInfo.ConstantBuffer().Resource() );

			m_shaderResources.BindResources( pipelineState.m_shaderState, snapshot.m_shaderBindings );

			VisibleDrawSnapshot visibleSnapshot = {
				0,
				0,
				1,
				-1,
				&snapshot,
			};

			VertexBuffer emptyPrimitiveID;
			CommitDrawSnapshot( commandList, visibleSnapshot, emptyPrimitiveID );
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

		PrimitiveSubMesh meshInfo;
		meshInfo.m_count = 3;

		auto result = skyAtmosphereDrawPassProcessor.Process( meshInfo );
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

		auto defaultSampler = GraphicsInterface().FindOrCreate( SamplerOption() );

		RenderingShaderResource skyAtmosphereDrawResources;
		skyAtmosphereDrawResources.AddResource( "TransmittanceLut", info->GetTransmittanceLutTexture()->SRV() );
		skyAtmosphereDrawResources.AddResource( "TransmittanceLutSampler", defaultSampler.Resource() );
		skyAtmosphereDrawResources.AddResource( "IrradianceLut", info->GetIrradianceLutTexture()->SRV() );
		skyAtmosphereDrawResources.AddResource( "IrradianceLutSampler", defaultSampler.Resource() );
		skyAtmosphereDrawResources.AddResource( "InscatterLut", info->GetInscatterLutTexture()->SRV() );
		skyAtmosphereDrawResources.AddResource( "InscatterLutSampler", defaultSampler.Resource() );
		skyAtmosphereDrawResources.AddResource( "SkyAtmosphereRenderParameter", skyAtmosphereRenderParameter.Resource() );

		skyAtmosphereDrawResources.BindResources( pipelineState.m_shaderState, snapshot.m_shaderBindings );

		VisibleDrawSnapshot visibleSnapshot = {
			0,
			0,
			1,
			-1,
			&snapshot,
		};

		VertexBuffer emptyPrimitiveID;
		CommitDrawSnapshot( commandList, visibleSnapshot, emptyPrimitiveID );
	}

	void SceneRenderer::RenderVolumetricCloud( IScene& scene, RenderView& renderView )
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

		PrimitiveSubMesh meshInfo;
		meshInfo.m_count = 3;

		auto result = volumetricCloundDrawPassProcessor.Process( meshInfo );
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
			.m_cameraPos = renderView.m_viewOrigin,
			.m_densityScale = proxy.DensityScale(),
			.m_lightPosOrDir = lightPosOrDir,
			.m_cloudColor = proxy.CloudColor(),
			.m_windDirection = Vector( 0.5f, 0.f, 0.1f ).GetNormalized(),
			.m_windSpeed = 450.f,
			.m_crispiness = proxy.Crispiness(),
			.m_curliness = proxy.Curliness(),
			.m_densityFactor = proxy.DensityFactor(),
		};

		auto& volumetricCloudRenderParameter = info->GetVolumetricCloudRenderParameter();
		volumetricCloudRenderParameter.Update( param );

		DrawSnapshot& snapshot = *result;

		// Update invalidated resources
		GraphicsPipelineState& pipelineState = snapshot.m_pipelineState;
		m_shaderResources.BindResources( pipelineState.m_shaderState, snapshot.m_shaderBindings );

		auto commandList = GetCommandList();
		ApplyOutputContext( commandList );

		SamplerOption samplerOption;
		samplerOption.m_addressU =
			samplerOption.m_addressV =
			samplerOption.m_addressW = agl::TextureAddressMode::Wrap;
		SamplerState cloudSampler = GraphicsInterface().FindOrCreate( samplerOption );
		SamplerState weatherSampler = GraphicsInterface().FindOrCreate( samplerOption );

		RenderingShaderResource volumetricCloundDrawResources;
		volumetricCloundDrawResources.AddResource( "VolumetricCloudRenderParameter", volumetricCloudRenderParameter.Resource() );
		volumetricCloundDrawResources.AddResource( "BaseCloudShape", info->BaseCloudShape()->SRV() );
		volumetricCloundDrawResources.AddResource( "DetailCloudShape", info->DetailCloudShape()->SRV() );
		volumetricCloundDrawResources.AddResource( "WeatherMap", info->WeatherMap()->SRV() );
		volumetricCloundDrawResources.AddResource( "BaseCloudSampler", cloudSampler.Resource() );
		volumetricCloundDrawResources.AddResource( "WeatherSampler", weatherSampler.Resource() );

		volumetricCloundDrawResources.BindResources( pipelineState.m_shaderState, snapshot.m_shaderBindings );

		VisibleDrawSnapshot visibleSnapshot = {
			0,
			0,
			1,
			-1,
			&snapshot,
		};

		VertexBuffer emptyPrimitiveID;
		CommitDrawSnapshot( commandList, visibleSnapshot, emptyPrimitiveID );
	}

	void SceneRenderer::RenderTemporalAntiAliasing( RenderViewGroup& renderViewGroup )
	{
		m_taa.Render( GetRenderRenderTargets(), renderViewGroup );
	}

	void SceneRenderer::StoreOuputContext( const RenderingOutputContext& context )
	{
		m_outputContext = context;
	}
}
