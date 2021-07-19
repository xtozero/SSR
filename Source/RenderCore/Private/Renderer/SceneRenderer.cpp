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

#include <deque>

void SceneRenderer::WaitUntilRenderingIsFinish( )
{
	PrimitiveIdVertexBufferPool::GetInstance( ).DiscardAll( );
}

void SceneRenderer::RenderTexturedSky( IScene& scene )
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

	StaticMeshLODResource& lodResource = renderData->LODResource( 0 );
	VertexLayoutDesc vertexLayoutDesc = renderData->VertexLayout( 0 ).Desc( );

	for ( const auto& section : lodResource.m_sections )
	{
		DrawSnapshot snapshot;
		snapshot.m_vertexStream.Bind( lodResource.m_vb, 0 );
		snapshot.m_indexBuffer = lodResource.m_ib;

		auto& pipelineState = snapshot.m_pipelineState;
		material->TakeSnapShot( snapshot );

		auto& graphicsInterface = GraphicsInterface( );
		if ( pipelineState.m_shaderState.m_vertexShader )
		{
			pipelineState.m_shaderState.m_vertexLayout = graphicsInterface.FindOrCreate( *pipelineState.m_shaderState.m_vertexShader, vertexLayoutDesc );

			const auto& vsParameterMap = snapshot.m_pipelineState.m_shaderState.m_vertexShader->ParameterMap( );

			auto binding = snapshot.m_shaderBindings.GetSingleShaderBindings( SHADER_TYPE::VS );
			aga::ShaderParameter viewProjection = vsParameterMap.GetParameter( "VEIW_PROJECTION" );
			binding.AddConstantBuffer( viewProjection, viewConstant.Resource( ) );
		}

		pipelineState.m_depthStencilState = graphicsInterface.FindOrCreate( proxy->GetDepthStencilOption( ) );
		pipelineState.m_rasterizerState = graphicsInterface.FindOrCreate( proxy->GetRasterizerOption( ) );

		pipelineState.m_primitive = RESOURCE_PRIMITIVE::TRIANGLELIST;

		snapshot.m_indexCount = section.m_count;
		snapshot.m_startIndexLocation = section.m_startLocation;
		snapshot.m_baseVertexLocation = 0;

		PreparePipelineStateObject( snapshot );

		VisibleDrawSnapshot visibleSnapshot = {
			0,
			0,
			0,
			1,
			&snapshot,
		};

		VertexBuffer emptyPrimitiveID;
		CommitDrawSnapshot( visibleSnapshot, emptyPrimitiveID );
	}
}

void SceneRenderer::RenderMesh( IScene& scene, RenderView& view )
{
	const auto& primitives = scene.Primitives( );
	if ( primitives.size( ) == 0 )
	{
		return;
	}

	auto* renderScene = scene.GetRenderScene( );
	if ( renderScene == nullptr )
	{
		return;
	}

	auto& viewConstant = scene.SceneViewConstant( );

	std::deque<DrawSnapshot> snapshotStorage;

	// Create DrawSnapshot
	for ( auto primitive : primitives )
	{
		PrimitiveProxy* proxy = primitive->m_sceneProxy;

		const std::vector<PrimitiveSubMeshInfo>& subMeshInfos = primitive->SubMeshInfos( );

		if ( subMeshInfos.size( ) > 0 )
		{
			for ( const auto& subMeshInfo : subMeshInfos )
			{
				std::size_t snapshotIndex = subMeshInfo.m_snapshotInfoBase;
				DrawSnapshot& snapshot = primitive->CachedDrawSnapshot( snapshotIndex );

				VisibleDrawSnapshot& visibleSnapshot = view.m_snapshots.emplace_back( );
				visibleSnapshot.m_drawSnapshot = &snapshot;
				visibleSnapshot.m_primitiveId = proxy->PrimitiveId( );
				visibleSnapshot.m_primitiveIdStreamSlot = 1;
				visibleSnapshot.m_numInstances = 1;
			}
		}
		else
		{
			proxy->TakeSnapshot( snapshotStorage, viewConstant, view.m_snapshots );
		}
	}

	// Update new primitvie info buffer
	auto& gpuPrimitiveInfo = scene.GpuPrimitiveInfo( );
	for ( auto& viewDrawSnapshot : view.m_snapshots )
	{
		DrawSnapshot& snapshot = *viewDrawSnapshot.m_drawSnapshot;
		GraphicsPipelineState& pipelineState = snapshot.m_pipelineState;
		const auto& vsParameterMap = pipelineState.m_shaderState.m_vertexShader->ParameterMap( );

		auto binding = snapshot.m_shaderBindings.GetSingleShaderBindings( SHADER_TYPE::VS );
		aga::ShaderParameter primitiveInfo = vsParameterMap.GetParameter( "primitiveInfo" );
		auto srv = gpuPrimitiveInfo.SRV( );
		binding.AddSRV( primitiveInfo, srv );
	}

	VertexBuffer primitiveIds = PrimitiveIdVertexBufferPool::GetInstance( ).Alloc( view.m_snapshots.size( ) * sizeof( UINT ) );

	SortDrawSnapshots( view.m_snapshots, primitiveIds );
	CommitDrawSnapshots( view.m_snapshots, primitiveIds );
}