#include "stdafx.h"
#include "Renderer/SceneRenderer.h"

#include "CommandList.h"
#include "Math/TransformationMatrix.h"
#include "Mesh/StaticMeshResource.h"
#include "Material/MaterialResource.h"
#include "Physics/CollideNarrow.h"
#include "Physics/Frustum.h"
#include "Proxies/LightProxy.h"
#include "Proxies/PrimitiveProxy.h"
#include "Proxies/TexturedSkyProxy.h"
#include "RenderView.h"
#include "Scene/IScene.h"
#include "Scene/PrimitiveSceneInfo.h"
#include "Scene/Scene.h"
#include "Scene/SceneConstantBuffers.h"
#include "ShaderBindings.h"
#include "ShadowDrawPassProcessor.h"
#include "ShadowSetup.h"
#include "SkyAtmosphereRendering.h"
#include "Viewport.h"

#include <deque>

void RenderingShaderResource::BindResources( const ShaderStates& shaders, aga::ShaderBindings& bindings )
{
	const ShaderBase* shaderArray[] = {
		shaders.m_vertexShader,
		nullptr, // HS
		nullptr, // DS
		shaders.m_geometryShader,
		shaders.m_pixelShader,
		nullptr, // CS
	};

	for ( uint32 shaderType = 0; shaderType < MAX_SHADER_TYPE<uint32>; ++shaderType )
	{
		if ( shaderArray[shaderType] == nullptr )
		{
			continue;
		}

		aga::SingleShaderBindings singleBinding = bindings.GetSingleShaderBindings( static_cast<SHADER_TYPE>( shaderType ) );

		if ( singleBinding.ShaderType() == SHADER_TYPE::NONE )
		{
			continue;
		}

		const auto& parameterMap = shaderArray[shaderType]->ParameterMap();

		for ( size_t i = 0; i < m_parameterNames.size(); ++i )
		{
			GraphicsApiResource* resource = m_resources[i];
			if ( resource == nullptr )
			{
				continue;
			}

			aga::ShaderParameter parameter = parameterMap.GetParameter( m_parameterNames[i].c_str() );

			switch ( parameter.m_type )
			{
			case aga::ShaderParameterType::ConstantBuffer:
				singleBinding.AddConstantBuffer( parameter, reinterpret_cast<aga::Buffer*>( m_resources[i] ) );
				break;
			case aga::ShaderParameterType::SRV:
				singleBinding.AddSRV( parameter, reinterpret_cast<aga::ShaderResourceView*>( m_resources[i] ) );
				break;
			case aga::ShaderParameterType::UAV:
				singleBinding.AddUAV( parameter, reinterpret_cast<aga::UnorderedAccessView*>( m_resources[i] ) );
				break;
			case aga::ShaderParameterType::Sampler:
				singleBinding.AddSampler( parameter, reinterpret_cast<aga::SamplerState*>( m_resources[i] ) );
				break;
			default:
				break;
			}
		}
	}
}

void RenderingShaderResource::AddResource( const std::string& parameterName, GraphicsApiResource* resource )
{
	auto found = std::find( std::begin( m_parameterNames ), std::end( m_parameterNames ), parameterName );

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
	m_shaderResources.ClearResources();

	std::destroy_at( &m_passSnapshots );
	std::destroy_at( &m_shadowInfos );

	rendercore::Allocator().Flush();
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

	rendercore::VectorSingleFrame<ShadowInfo*> viewDependentShadow;

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

void SceneRenderer::ClassifyShadowCasterAndReceiver( IScene& scene, const rendercore::VectorSingleFrame<ShadowInfo*>& shadows )
{
	using namespace DirectX;

	Scene& renderScene = *scene.GetRenderScene();

	for ( ShadowInfo* pShadowInfo : shadows )
	{
		LightSceneInfo* lightSceneInfo = pShadowInfo->GetLightSceneInfo();
		LIGHT_TYPE lightType = lightSceneInfo->Proxy()->LightType();

		assert( lightType == LIGHT_TYPE::DIRECTINAL_LIGHT );
		assert( pShadowInfo->View() != nullptr );

		const RenderView& view = *pShadowInfo->View();
		auto viewMat = LookFromMatrix( view.m_viewOrigin, view.m_viewAxis[2], view.m_viewAxis[1] );
		auto viewProjectionMat = PerspectiveMatrix( view.m_fov, view.m_aspect, view.m_nearPlaneDistance, view.m_farPlaneDistance );
		viewProjectionMat = viewMat * viewProjectionMat;
		Frustum frustum( viewProjectionMat );

		CAaboundingbox box;

		const Vector& lightDirection = lightSceneInfo->Proxy()->GetLightProperty().m_direction;
		Vector sweepDir = lightDirection.GetNormalized();

		const auto& intersectionInfos = lightSceneInfo->Primitives();
		for ( const auto& intersectionInfo : intersectionInfos )
		{
			PrimitiveSceneInfo* primitive = intersectionInfo.m_primitive;
			uint32 id = primitive->PrimitiveId();

			const BoxSphereBounds& bounds = renderScene.PrimitiveBounds()[id];

			uint32 inFrustum = BoxAndFrustum( bounds.Origin() - bounds.HalfSize(),
				bounds.Origin() + bounds.HalfSize(),
				frustum );

			BoxSphereBounds viewspaceBounds = bounds.TransformBy( viewMat );

			switch ( inFrustum )
			{
			case COLLISION::OUTSIDE:
			{
				bool isIntersected = SphereAndFrusturm( bounds.Origin(), bounds.Radius(), frustum, sweepDir );
				if ( isIntersected )
				{
					pShadowInfo->AddCasterPrimitive( primitive, viewspaceBounds );
				}
			}
			break;
			case COLLISION::INSIDE:
			case COLLISION::INTERSECTION:
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
		LIGHT_TYPE lightType = lightSceneInfo->Proxy()->LightType();

		switch ( lightType )
		{
		case LIGHT_TYPE::DIRECTINAL_LIGHT:
		{
			BuildOrthoShadowProjectionMatrix( shadowInfo );
			break;
		}
		case LIGHT_TYPE::POINT_LIGHT:
			break;
		case LIGHT_TYPE::SPOT_LIGHT:
			break;
		case LIGHT_TYPE::NONE:
		default:
			break;
		}
	}
}

void SceneRenderer::AllocateShadowMaps()
{
	rendercore::VectorSingleFrame<ShadowInfo*> cascadeShadows;

	for ( auto& shadowInfo : m_shadowInfos )
	{
		LightSceneInfo* lightSceneInfo = shadowInfo.GetLightSceneInfo();
		LIGHT_TYPE lightType = lightSceneInfo->Proxy()->LightType();

		switch ( lightType )
		{
		case LIGHT_TYPE::NONE:
			break;
		case LIGHT_TYPE::DIRECTINAL_LIGHT:
			cascadeShadows.push_back( &shadowInfo );
			break;
		case LIGHT_TYPE::POINT_LIGHT:
			break;
		case LIGHT_TYPE::SPOT_LIGHT:
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

void SceneRenderer::AllocateCascadeShadowMaps( const rendercore::VectorSingleFrame<ShadowInfo*>& shadows )
{
	for ( ShadowInfo* shadow : shadows )
	{
		auto [width, height] = shadow->ShadowMapSize();

		TEXTURE_TRAIT trait = { width,
								height,
								CascadeShadowSetting::MAX_CASCADE_NUM, // Cascade map count, Right now, it's fixed constant.
								1,
								0,
								1,
								RESOURCE_FORMAT::R32_FLOAT,
								RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::GPU_WRITE,
								RESOURCE_BIND_TYPE::RENDER_TARGET | RESOURCE_BIND_TYPE::SHADER_RESOURCE,
								0 };

		shadow->ShadowMap().m_shadowMap = aga::Texture::Create( trait );

		TEXTURE_TRAIT depthTrait = { width,
									height,
									CascadeShadowSetting::MAX_CASCADE_NUM, // Cascade map count, Right now, it's fixed constant.
									1,
									0,
									1,
									RESOURCE_FORMAT::D24_UNORM_S8_UINT,
									RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::GPU_WRITE,
									RESOURCE_BIND_TYPE::DEPTH_STENCIL,
									0 };

		shadow->ShadowMap().m_shadowMapDepth = aga::Texture::Create( depthTrait );

		EnqueueRenderTask( [texture = shadow->ShadowMap().m_shadowMap,
			depthTexture = shadow->ShadowMap().m_shadowMapDepth]()
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
		auto [width, height] = shadowInfo.ShadowMapSize();

		RenderingOutputContext context = {
			{ shadowMap.m_shadowMap },
			shadowMap.m_shadowMapDepth,
			{ 0.f, 0.f, static_cast<float>( width ), static_cast<float>( height ), 0.f, 1.f },
			{ 0L, 0L, static_cast<int32>( width ), static_cast<int32>( height ) }
		};
		StoreOuputContext( context );

		auto commandList = rendercore::GetImmediateCommandList();
		commandList.ClearRenderTarget( shadowMap.m_shadowMap, { 1, 1, 1, 1 } );
		commandList.ClearDepthStencil( shadowMap.m_shadowMapDepth, 1.f, 0 );

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

	auto commandList = rendercore::GetImmediateCommandList();
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

		pipelineState.m_primitive = RESOURCE_PRIMITIVE::TRIANGLELIST;

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

		auto commandList = rendercore::GetImmediateCommandList();
		ApplyOutputContext( commandList );

		m_shaderResources.AddResource( "ShadowTexture", shadowInfo.ShadowMap().m_shadowMap->SRV() );

		SamplerOption shadowSamplerOption;
		shadowSamplerOption.m_addressU = TEXTURE_ADDRESS_MODE::BORDER;
		shadowSamplerOption.m_addressV = TEXTURE_ADDRESS_MODE::BORDER;
		shadowSamplerOption.m_addressW = TEXTURE_ADDRESS_MODE::BORDER;
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

	rendercore::SkyAtmosphereRenderSceneInfo* info = renderScene->SkyAtmosphereSceneInfo();
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
	rendercore::SkyAtmosphereRenderParameter param = {
		.m_cameraPos = renderView.m_viewOrigin,
		.m_sunDir = -lightProperty.m_direction,
		.m_exposure = 0.4f,
	};
	skyAtmosphereRenderParameter.Update( param );

	rendercore::SkyAtmosphereDrawPassProcessor skyAtmosphereDrawPassProcessor;

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

	auto commandList = rendercore::GetImmediateCommandList();
	ApplyOutputContext( commandList );

	auto defaultSampler = GraphicsInterface().FindOrCreate( SamplerOption() );

	RenderingShaderResource skyAtmosphereDrawResources;
	skyAtmosphereDrawResources.AddResource( "TransmittanceLut", info->GetTransmittanceLutTexture()->SRV() );
	skyAtmosphereDrawResources.AddResource( "TransmittanceLutSampler", defaultSampler.Resource() );
	skyAtmosphereDrawResources.AddResource( "IrradianceLut", info->GetIrradianceLutTexture()->SRV() );
	skyAtmosphereDrawResources.AddResource( "IrradianceLutSampler" , defaultSampler.Resource() );
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

void SceneRenderer::StoreOuputContext( const RenderingOutputContext& context )
{
	m_outputContext = context;
}
