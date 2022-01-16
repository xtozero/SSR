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

StaticMeshPrimitiveProxy::StaticMeshPrimitiveProxy( const StaticMeshComponent& component ) : m_pStaticMesh( component.GetStaticMesh() ), m_pRenderData( m_pStaticMesh->RenderData() ), m_pRenderOption( component.GetRenderOption() )
{

}

void StaticMeshPrimitiveProxy::CreateRenderData()
{
	assert( IsInRenderThread() );
	m_pRenderData->CreateRenderResource();
}

void StaticMeshPrimitiveProxy::PrepareSubMeshs()
{
	assert( IsInRenderThread() );
	uint32 lodSize = m_pRenderData->LODSize();

	for ( uint32 lod = 0; lod < lodSize; ++lod )
	{
		StaticMeshLODResource& lodResource = m_pRenderData->LODResource( lod );

		uint32 sectionSize = static_cast<uint32>( lodResource.m_sections.size() );
		for ( uint32 sectionIndex = 0; sectionIndex < sectionSize; ++sectionIndex )
		{
			PrimitiveSubMesh& subMesh = m_primitiveSceneInfo->AddSubMesh();
			new ( &subMesh ) PrimitiveSubMesh( GatherMeshDrawInfo( lod, sectionIndex ) );
		}
	}
}

void StaticMeshPrimitiveProxy::TakeSnapshot( std::deque<DrawSnapshot>& snapshotStorage, rendercore::VectorSingleFrame<VisibleDrawSnapshot>& drawList ) const
{
	// To Do : will make lod available later
	StaticMeshLODResource& lodResource = m_pRenderData->LODResource( 0 );
	uint32 sectionSize = static_cast<uint32>( lodResource.m_sections.size() );

	for ( uint32 sectionIndex = 0; sectionIndex < sectionSize; ++sectionIndex )
	{
		std::optional<DrawSnapshot> snapshot = TakeSnapshot( 0, sectionIndex );

		if ( snapshot )
		{
			snapshotStorage.emplace_back( std::move( snapshot.value() ) );
			VisibleDrawSnapshot& visibleSnapshot = drawList.emplace_back();
			visibleSnapshot.m_primitiveId = PrimitiveId();
			visibleSnapshot.m_numInstance = 1;
			visibleSnapshot.m_snapshotBucketId = -1;
			visibleSnapshot.m_drawSnapshot = &snapshotStorage.back();
		}
	}
}

std::optional<DrawSnapshot> StaticMeshPrimitiveProxy::TakeSnapshot( uint32 lod, uint32 sectionIndex ) const
{
	assert( IsInRenderThread() );
	uint32 lodSize = m_pRenderData->LODSize();
	if ( lod >= lodSize )
	{
		return {};
	}

	auto& graphicsInterface = GraphicsInterface();

	StaticMeshLODResource& lodResource = m_pRenderData->LODResource( lod );
	const VertexCollection& vertexCollection = lodResource.m_vertexCollection;
	VertexStreamLayout vertexlayout = vertexCollection.VertexLayout( VertexStreamLayoutType::Default );

	uint32 primitiveIdSlot = vertexlayout.Size();
	vertexlayout.AddLayout( "PRIMITIVEID", 0,
		RESOURCE_FORMAT::R32_UINT,
		primitiveIdSlot,
		true,
		1,
		-1 );

	DrawSnapshot snapshot;
	vertexCollection.Bind( snapshot.m_vertexStream, VertexStreamLayoutType::Default );
	snapshot.m_primitiveIdSlot = primitiveIdSlot;
	snapshot.m_indexBuffer = lodResource.m_ib;

	GraphicsPipelineState& pipelineState = snapshot.m_pipelineState;
	const StaticMeshSection& section = lodResource.m_sections[sectionIndex];
	auto materialResource = m_pStaticMesh->GetMaterialResource( section.m_materialIndex );
	if ( materialResource )
	{
		pipelineState.m_shaderState.m_vertexShader = materialResource->GetVertexShader();
		pipelineState.m_shaderState.m_geometryShader = materialResource->GetGeometryShader();
		pipelineState.m_shaderState.m_pixelShader = materialResource->GetPixelShader();

		auto initializer = CreateShaderBindingsInitializer( pipelineState.m_shaderState );
		snapshot.m_shaderBindings.Initialize( initializer );

		materialResource->TakeSnapshot( snapshot );
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

	snapshot.m_count = section.m_count;
	snapshot.m_startIndexLocation = section.m_startLocation;
	snapshot.m_baseVertexLocation = 0;

	if ( pipelineState.m_shaderState.m_vertexShader )
	{
		pipelineState.m_shaderState.m_vertexLayout = graphicsInterface.FindOrCreate( *pipelineState.m_shaderState.m_vertexShader, vertexlayout );
	}

	PreparePipelineStateObject( snapshot );

	return snapshot;
}

MeshDrawInfo StaticMeshPrimitiveProxy::GatherMeshDrawInfo( uint32 lod, uint32 sectionIndex ) const
{
	uint32 lodSize = m_pRenderData->LODSize();
	if ( lod >= lodSize )
	{
		return {};
	}

	StaticMeshLODResource& lodResource = m_pRenderData->LODResource( lod );
	if ( sectionIndex >= lodResource.m_sections.size() )
	{
		return {};
	}

	const StaticMeshSection& section = lodResource.m_sections[sectionIndex];

	MeshDrawInfo info;

	info.m_vertexCollection = &lodResource.m_vertexCollection;
	info.m_indexBuffer = &lodResource.m_ib;
	info.m_material = m_pStaticMesh->GetMaterialResource( section.m_materialIndex );
	info.m_renderOption = &*m_pRenderOption;

	info.m_startLocation = section.m_startLocation;
	info.m_count = section.m_count;

	info.m_lod = lod;
	info.m_sectionIndex = sectionIndex;

	return info;
}
