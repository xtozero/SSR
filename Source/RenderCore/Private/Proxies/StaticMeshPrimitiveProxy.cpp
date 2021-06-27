#include "stdafx.h"
#include "Proxies/StaticMeshPrimitiveProxy.h"

#include "AbstractGraphicsInterface.h"
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
	if ( lodSize == 0 )
	{
		return;
	}

	auto& graphicsInterface = GraphicsInterface( );

	// To Do : will make lod available later
	StaticMeshLODResource& lodResource = m_pRenderData->LODResource( 0 );
	VertexLayoutDesc vertexLayoutDesc = m_pRenderData->VertexLayout( 0 ).Desc( );

	vertexLayoutDesc.AddLayout( "PRIMITIVEID", 0,
								RESOURCE_FORMAT::R32_UINT,
								1,
								true,
								1 );

	for ( const auto& section : lodResource.m_sections )
	{
		DrawSnapshot& snapshot = snapshots.emplace_back( );
		snapshot.m_primitiveId = GetId( );
		snapshot.m_vertexStream.Bind( lodResource.m_vb, 0 );
		snapshot.m_indexBuffer = lodResource.m_ib;

		GraphicsPipelineState& pipelineState = snapshot.m_pipelineState;
		auto materialResource = m_pStaticMesh->GetMaterialResource( section.m_materialIndex );
		if ( materialResource )
		{
			materialResource->TakeSnapShot( snapshot );
			pipelineState.m_shaderState.m_vertexLayout = graphicsInterface.FindOrCreate( pipelineState.m_shaderState.m_vertexShader, vertexLayoutDesc );
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
	}
}
