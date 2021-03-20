#include "stdafx.h"
#include "Proxies/StaticMeshPrimitiveProxy.h"

#include "AgaDelegator.h"
#include "Components/StaticMeshComponent.h"
#include "DrawSnapshot.h"
#include "Mesh/StaticMeshResource.h"
#include "Model/StaticMesh.h"
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

void StaticMeshPrimitiveProxy::TakeSnapshot( std::vector<DrawSnapshot>& snapshots )
{
	assert( IsInRenderThread( ) );
	std::size_t lodSize = m_pRenderData->LODSize( );
	if ( lodSize < 0 )
	{
		return;
	}

	auto& aga = GetAgaDelegator( );

	// To Do : will make lod available later
	const StaticMeshLODResource& lodResource = m_pRenderData->LODResource( 0 );
	const StaticMeshVertexLayout& vertexLayout = m_pRenderData->VertexLayout( 0 );

	for ( const auto& section : lodResource.m_sections )
	{
		DrawSnapshot& snapShot = snapshots.emplace_back( );
		snapShot.m_vertexStream.Bind( lodResource.m_vb, 0 );
		snapShot.m_indexBuffer = lodResource.m_ib;

		snapShot.m_shaders.m_vertexLayout = aga.FindOrCreate( *m_pRenderOption->m_vertexShader, vertexLayout.Desc( ) );
		snapShot.m_shaders.m_vertexShader = *m_pRenderOption->m_vertexShader;
		snapShot.m_shaders.m_pixelShader = *m_pRenderOption->m_pixelShader;

		GraphicsPipelineState pipelineState = snapShot.m_pipelineState;
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