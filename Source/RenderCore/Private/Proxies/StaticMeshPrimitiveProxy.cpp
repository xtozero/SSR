#include "stdafx.h"
#include "Proxies/StaticMeshPrimitiveProxy.h"

#include "AgaDelegator.h"
#include "Components/StaticMeshComponent.h"
#include "DrawSnapshot.h"
#include "Material/MaterialResource.h"
#include "Mesh/StaticMesh.h"
#include "Mesh/StaticMeshResource.h"
#include "MultiThread/EngineTaskScheduler.h"
#include "RenderOption.h"

StaticMeshPrimitiveProxy::StaticMeshPrimitiveProxy( const StaticMeshComponent& component ) : m_pStaticMesh( component.GetStaticMesh( ) ), m_pRenderData( m_pStaticMesh->RenderData( ) ), m_pRenderOption( component.GetRenderOption( ) )
{
	
}

void StaticMeshPrimitiveProxy::CreateRenderData( )
{
	assert( IsInRenderThread( ) );
	m_pRenderData->CreateRenderResource( );
}

void StaticMeshPrimitiveProxy::TakeSnapshot( std::vector<DrawSnapshot>& snapshots ) const
{
	assert( IsInRenderThread( ) );
	std::size_t lodSize = m_pRenderData->LODSize( );
	if ( lodSize < 0 )
	{
		return;
	}

	auto& aga = GetAgaDelegator( );

	// To Do : will make lod available later
	StaticMeshLODResource& lodResource = m_pRenderData->LODResource( 0 );
	const StaticMeshVertexLayout& vertexLayout = m_pRenderData->VertexLayout( 0 );

	for ( const auto& section : lodResource.m_sections )
	{
		DrawSnapshot& snapShot = snapshots.emplace_back( );
		snapShot.m_primitiveId = GetId( );
		snapShot.m_vertexStream.Bind( lodResource.m_vb, 0 );
		snapShot.m_indexBuffer = lodResource.m_ib;

		GraphicsPipelineState& pipelineState = snapShot.m_pipelineState;
		auto materialResource = m_pStaticMesh->GetMaterialResource( section.m_materialIndex );
		if ( materialResource )
		{
			materialResource->TakeSnapShot( snapShot );
			pipelineState.m_shaderState.m_vertexLayout = aga.FindOrCreate( pipelineState.m_shaderState.m_vertexShader, vertexLayout.Desc( ) );
		}

		if ( m_pRenderOption->m_blendOption )
		{
			pipelineState.m_blendState = aga.FindOrCreate( *m_pRenderOption->m_blendOption );
		}

		if ( m_pRenderOption->m_depthStencilOption )
		{
			pipelineState.m_depthStencilState = aga.FindOrCreate( *m_pRenderOption->m_depthStencilOption );
		}

		if ( m_pRenderOption->m_rasterizerOption )
		{
			pipelineState.m_rasterizerState = aga.FindOrCreate( *m_pRenderOption->m_rasterizerOption );
		}
		
		pipelineState.m_primitive = RESOURCE_PRIMITIVE::TRIANGLELIST;

		snapShot.m_indexCount = section.m_count;
		snapShot.m_startIndexLocation = section.m_startLocation;
		snapShot.m_baseVertexLocation = 0;
	}
}
