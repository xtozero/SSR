#include "stdafx.h"
#include "Proxies/StaticMeshPrimitiveProxy.h"

#include "AbstractGraphicsInterface.h"
#include "Components/StaticMeshComponent.h"
#include "DrawSnapshot.h"
#include "Material/MaterialResource.h"
#include "Mesh/StaticMesh.h"
#include "Mesh/StaticMeshResource.h"
#include "RenderOption.h"
#include "Scene/PrimitiveSceneInfo.h"
#include "Scene/SceneConstantBuffers.h"
#include "TaskScheduler.h"

StaticMeshPrimitiveProxy::StaticMeshPrimitiveProxy( const StaticMeshComponent& component ) : m_pStaticMesh( component.GetStaticMesh( ) ), m_pRenderData( m_pStaticMesh->RenderData( ) ), m_pRenderOption( component.GetRenderOption( ) )
{
	
}

void StaticMeshPrimitiveProxy::CreateRenderData( )
{
	assert( IsInRenderThread( ) );
	m_pRenderData->CreateRenderResource( );
}

void StaticMeshPrimitiveProxy::PrepareSubMeshs( )
{
	assert( IsInRenderThread( ) );
	uint32 lodSize = m_pRenderData->LODSize( );

	for ( uint32 lod = 0; lod < lodSize; ++lod )
	{
		StaticMeshLODResource& lodResource = m_pRenderData->LODResource( lod );

		uint32 sectionSize = static_cast<uint32>( lodResource.m_sections.size( ) );
		for ( uint32 sectionIndex = 0; sectionIndex < sectionSize; ++sectionIndex )
		{
			PrimitiveSubMesh& subMesh = m_primitiveSceneInfo->AddSubMesh( );
			GetSubMeshElement( lod, sectionIndex, subMesh );
		}
	}
}

void StaticMeshPrimitiveProxy::GetSubMeshElement( uint32 lod, uint32 sectionIndex, PrimitiveSubMesh& subMesh )
{
	StaticMeshLODResource& lodResource = m_pRenderData->LODResource( lod );
	const StaticMeshSection& section = lodResource.m_sections[sectionIndex];

	subMesh.Lod( ) = lod;
	subMesh.SectionIndex( ) = sectionIndex;
}

void StaticMeshPrimitiveProxy::TakeSnapshot( std::deque<DrawSnapshot>& snapshotStorage, SceneViewConstantBuffer& viewConstant, std::vector<VisibleDrawSnapshot>& drawList ) const
{
	// To Do : will make lod available later
	StaticMeshLODResource& lodResource = m_pRenderData->LODResource( 0 );
	uint32 sectionSize = static_cast<uint32>( lodResource.m_sections.size( ) );

	for ( uint32 sectionIndex = 0; sectionIndex < sectionSize; ++sectionIndex )
	{
		std::optional<DrawSnapshot> snapshot = TakeSnapshot( 0, sectionIndex, viewConstant );

		if ( snapshot )
		{
			snapshotStorage.emplace_back( std::move( snapshot.value( ) ) );
			VisibleDrawSnapshot& visibleSnapshot = drawList.emplace_back( );
			visibleSnapshot.m_primitiveId = PrimitiveId( );
			visibleSnapshot.m_numInstance = 1;
			visibleSnapshot.m_snapshotBucketId = -1;
			visibleSnapshot.m_drawSnapshot = &snapshotStorage.back( );
		}
	}
}

std::optional<DrawSnapshot> StaticMeshPrimitiveProxy::TakeSnapshot( uint32 lod, uint32 sectionIndex, SceneViewConstantBuffer& viewConstant ) const
{
	assert( IsInRenderThread( ) );
	uint32 lodSize = m_pRenderData->LODSize( );
	if ( lod >= lodSize )
	{
		return {};
	}

	auto& graphicsInterface = GraphicsInterface( );

	StaticMeshLODResource& lodResource = m_pRenderData->LODResource( lod );
	VertexLayoutDesc vertexLayoutDesc = m_pRenderData->VertexLayout( lod ).Desc( );

	vertexLayoutDesc.AddLayout( "PRIMITIVEID", 0,
								RESOURCE_FORMAT::R32_UINT,
								1,
								true,
								1 );

	const StaticMeshSection& section = lodResource.m_sections[sectionIndex];

	DrawSnapshot snapshot;
	snapshot.m_vertexStream.Bind( lodResource.m_vb, 0 );
	snapshot.m_indexBuffer = lodResource.m_ib;

	GraphicsPipelineState& pipelineState = snapshot.m_pipelineState;
	auto materialResource = m_pStaticMesh->GetMaterialResource( section.m_materialIndex );
	if ( materialResource )
	{
		materialResource->TakeSnapShot( snapshot );
	}

	if ( m_pRenderOption->m_blendOption )
	{
		pipelineState.m_blendState = graphicsInterface.FindOrCreate( *m_pRenderOption->m_blendOption );
	}

	if ( m_pRenderOption->m_depthStencilOption )
	{
		pipelineState.m_depthStencilState = graphicsInterface.FindOrCreate( *m_pRenderOption->m_depthStencilOption );
	}

	if ( m_pRenderOption->m_rasterizerOption )
	{
		pipelineState.m_rasterizerState = graphicsInterface.FindOrCreate( *m_pRenderOption->m_rasterizerOption );
	}

	pipelineState.m_primitive = RESOURCE_PRIMITIVE::TRIANGLELIST;

	snapshot.m_indexCount = section.m_count;
	snapshot.m_startIndexLocation = section.m_startLocation;
	snapshot.m_baseVertexLocation = 0;

	if ( pipelineState.m_shaderState.m_vertexShader )
	{
		pipelineState.m_shaderState.m_vertexLayout = graphicsInterface.FindOrCreate( *pipelineState.m_shaderState.m_vertexShader, vertexLayoutDesc );
	}

	PreparePipelineStateObject( snapshot );

	return snapshot;
}
