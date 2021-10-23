#include "stdafx.h"
#include "Renderer/SceneRenderer.h"

#include "Mesh/StaticMeshResource.h"
#include "Material/MaterialResource.h"
#include "Proxies/PrimitiveProxy.h"
#include "Proxies/TexturedSkyProxy.h"
#include "RenderView.h"
#include "Scene/IScene.h"
#include "Scene/PrimitiveSceneInfo.h"
#include "Scene/Scene.h"
#include "Scene/SceneConstantBuffers.h"
#include "ShaderBindings.h"
#include "Viewport.h"

#include <deque>

void RenderingShaderResource::BindResources( const ShaderStates& shaders, aga::ShaderBindings& bindings )
{
	const ShaderBase* shaderArray[] = {
		shaders.m_vertexShader,
		nullptr, // HS
		nullptr, // DS
		nullptr, // GS					
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

		if ( singleBinding.ShaderType( ) == SHADER_TYPE::NONE )
		{
			continue;
		}

		const auto& parameterMap = shaderArray[shaderType]->ParameterMap( );

		for ( size_t i = 0; i < m_parameterNames.size( ); ++i )
		{
			GraphicsApiResource* resource = m_resources[i];
			if ( resource == nullptr )
			{
				continue;
			}

			aga::ShaderParameter parameter = parameterMap.GetParameter( m_parameterNames[i].c_str( ) );
			
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

void RenderingShaderResource::ClearResources( )
{
	for ( auto& resource : m_resources )
	{
		resource = nullptr;
	}
}

void SceneRenderer::SetViewPort( aga::ICommandList& commandList, const RenderViewGroup& renderViewGroup )
{
	const auto& viewport = renderViewGroup.Viewport( );
	viewport.Bind( commandList );
}

void SceneRenderer::WaitUntilRenderingIsFinish( )
{
	PrimitiveIdVertexBufferPool::GetInstance( ).DiscardAll( );
}

void SceneRenderer::RenderTexturedSky( IScene& scene, RenderViewGroup& renderViewGroup )
{
	Scene* renderScene = scene.GetRenderScene( );
	if ( renderScene == nullptr )
	{
		return;
	}

	TexturedSkyProxy* proxy = renderScene->TexturedSky( );
	if ( proxy == nullptr )
	{
		return;
	}

	StaticMeshRenderData* renderData = proxy->GetRenderData( );
	MaterialResource* material = proxy->GetMaterialResource( );
	if ( ( renderData == nullptr ) || ( renderData->LODSize( ) == 0 || ( material == nullptr ) ) )
	{
		return;
	}

	auto& viewConstant = scene.SceneViewConstant( );
	auto commandList = GetInterface<aga::IAga>( )->GetImmediateCommandList( );

	SetRenderTarget( *commandList, renderViewGroup );
	SetViewPort( *commandList, renderViewGroup );

	StaticMeshLODResource& lodResource = renderData->LODResource( 0 );
	const VertexCollection& vertexCollection = lodResource.m_vertexCollection;

	for ( const auto& section : lodResource.m_sections )
	{
		DrawSnapshot snapshot;
		vertexCollection.Bind( snapshot.m_vertexStream, VertexStreamLayoutType::PositionOnly );
		snapshot.m_primitiveIdSlot = -1;
		snapshot.m_indexBuffer = lodResource.m_ib;

		material->TakeSnapShot( snapshot );

		auto& pipelineState = snapshot.m_pipelineState;
		auto& graphicsInterface = GraphicsInterface( );
		if ( pipelineState.m_shaderState.m_vertexShader )
		{
			VertexStreamLayout vertexlayout = vertexCollection.VertexLayout( VertexStreamLayoutType::PositionOnly );
			pipelineState.m_shaderState.m_vertexLayout = graphicsInterface.FindOrCreate( *pipelineState.m_shaderState.m_vertexShader, vertexlayout );
		}

		pipelineState.m_depthStencilState = graphicsInterface.FindOrCreate( proxy->GetDepthStencilOption( ) );
		pipelineState.m_rasterizerState = graphicsInterface.FindOrCreate( proxy->GetRasterizerOption( ) );

		pipelineState.m_primitive = RESOURCE_PRIMITIVE::TRIANGLELIST;

		snapshot.m_indexCount = section.m_count;
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
		CommitDrawSnapshot( *commandList, visibleSnapshot, emptyPrimitiveID );
	}
}

void SceneRenderer::RenderMesh( IScene& scene, RenderViewGroup& renderViewGroup, size_t curView )
{
	const auto& primitives = scene.Primitives( );
	if ( primitives.Size( ) == 0 )
	{
		return;
	}

	auto* renderScene = scene.GetRenderScene( );
	if ( renderScene == nullptr )
	{
		return;
	}

	auto& viewConstant = scene.SceneViewConstant( );
	auto& view = renderViewGroup[curView];

	std::deque<DrawSnapshot> snapshotStorage;

	// Create DrawSnapshot
	for ( auto primitive : primitives )
	{
		PrimitiveProxy* proxy = primitive->Proxy( );

		const std::vector<PrimitiveSubMeshInfo>& subMeshInfos = primitive->SubMeshInfos( );

		if ( subMeshInfos.size( ) > 0 )
		{
			for ( const auto& subMeshInfo : subMeshInfos )
			{
				uint32 snapshotIndex = subMeshInfo.m_snapshotInfoBase;
				const CachedDrawSnapshotInfo& info = primitive->GetCachedDrawSnapshotInfo( snapshotIndex );
				DrawSnapshot& snapshot = primitive->CachedDrawSnapshot( snapshotIndex );

				VisibleDrawSnapshot& visibleSnapshot = view.m_snapshots.emplace_back( );
				visibleSnapshot.m_snapshotBucketId = info.m_snapshotBucketId;
				visibleSnapshot.m_drawSnapshot = &snapshot;
				visibleSnapshot.m_primitiveId = proxy->PrimitiveId( );
				visibleSnapshot.m_numInstance = 1;
			}
		}
		else
		{
			proxy->TakeSnapshot( snapshotStorage, viewConstant, view.m_snapshots );
		}
	}

	// Update invalidated resources
	for ( auto& viewDrawSnapshot : view.m_snapshots )
	{
		DrawSnapshot& snapshot = *viewDrawSnapshot.m_drawSnapshot;
		GraphicsPipelineState& pipelineState = snapshot.m_pipelineState;

		m_shaderResources.BindResources( pipelineState.m_shaderState, snapshot.m_shaderBindings );
	}

	VertexBuffer primitiveIds = PrimitiveIdVertexBufferPool::GetInstance( ).Alloc( static_cast<uint32>( view.m_snapshots.size( ) * sizeof( uint32 ) ) );

	SortDrawSnapshots( view.m_snapshots, primitiveIds );
	// CommitDrawSnapshots( *this, renderViewGroup, curView, primitiveIds );
	ParallelCommitDrawSnapshot( *this, renderViewGroup, curView, primitiveIds );
}
