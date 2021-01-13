#include "stdafx.h"
#include "Model/StaticMesh.h"

#include "common.h"
#include "Mesh/StaticMeshResource.h"
#include "Model/CommonMeshDefine.h"
#include "Model/MeshDescription.h"
#include "MultiThread/EngineTaskScheduler.h"

Archive operator<<( Archive& ar, StaticMeshMaterial& m )
{
	ar << m.m_mateiral;
	return ar;
}

REGISTER_ASSET( StaticMesh );
void StaticMesh::Serialize( Archive& ar )
{
	if ( ar.IsWriteMode( ) )
	{
		ar << ID;
	}

	if ( m_renderData )
	{
		m_renderData->Serialize( ar );
	}

	if ( ar.IsWriteMode( ) )
	{
		ar << m_materials.size( );
	}
	else
	{
		std::size_t size = 0;
		ar << size;
		m_materials.resize( size );
	}

	for ( auto& material : m_materials )
	{
		ar << material;
	}
}

void StaticMesh::BuildMeshFromMeshDescriptions( const std::vector<MeshDescription>& meshDescriptions )
{
	m_renderData->AllocateLODResources( meshDescriptions.size( ) );

	int lodIndex = 0;
	for ( const auto& meshDescription : meshDescriptions )
	{
		StaticMeshLODResource& lodResource = m_renderData->LODResource( lodIndex );
		BuildMeshFromMeshDescription( meshDescription, lodResource );
	}
}

void StaticMesh::BuildMeshFromMeshDescription( const MeshDescription& meshDescription, StaticMeshLODResource& lodResource )
{
	const std::vector<CXMFLOAT3>& pos = meshDescription.m_positions;
	const std::vector<CXMFLOAT3>& normal = meshDescription.m_normals;
	const std::vector<CXMFLOAT2>& texCoord = meshDescription.m_texCoords;
	const std::vector<MeshVertexInstance>& vertexInstances = meshDescription.m_vertexInstances;

	auto& vertices = lodResource.m_vertexData;
	vertices.reserve( vertexInstances.size( ) );

	std::size_t normalCount = normal.size( );
	std::size_t texcoordCount = texCoord.size( );

	for ( std::size_t i = 0; i < vertexInstances.size( ); ++i )
	{
		const MeshVertexInstance& vertexInstance = vertexInstances[i];

		StaticMeshVertex& v = vertices.emplace_back( );
		v.m_position = pos[vertexInstance.m_positionID];
		v.m_normal = ( vertexInstance.m_normalID < normalCount ) ? normal[vertexInstance.m_normalID] : CXMFLOAT3( 0.f, 0.f, 0.f );
		v.m_texcoord = ( vertexInstance.m_texCoordID < texcoordCount ) ? texCoord[vertexInstance.m_texCoordID] : CXMFLOAT2( 0.f, 0.f );
	}

	const std::vector<MeshTriangle>& triangles = meshDescription.m_triangles;
	const std::vector<MeshPolygon>& polygons = meshDescription.m_polygons;

	std::size_t indexCount = 0;
	for ( const MeshPolygon& polygon : polygons )
	{
		indexCount += polygon.m_triangleID.size( ) * 3;
	}

	auto& indices = lodResource.m_indexData;
	indices.reserve( indexCount );

	for ( std::size_t i = 0; i < polygons.size( ); ++i )
	{
		const MeshPolygon& polygon = polygons[i];

		StaticMeshSection& section = lodResource.m_sections.emplace_back( );
		section.m_startLocation = indices.size( );

		for ( std::size_t triangleID : polygon.m_triangleID )
		{
			for ( std::size_t vertexInstanceID : triangles[triangleID].m_vertexInstanceID )
			{
				indices.push_back( vertexInstanceID );
			}
		}

		section.m_count = indices.size( ) - section.m_startLocation;

		const std::string& polygonMaterialName = meshDescription.m_polygonMaterialName[i];
		for ( std::size_t j = 0; j < m_materials.size( ); ++j )
		{
			const auto material = m_materials[j].m_mateiral;
			if ( polygonMaterialName == material->Path().stem() )
			{
				section.m_materialIndex = j;
				break;
			}
		}
	}
}

void StaticMesh::AddMaterial( const std::shared_ptr<Material>& mateiral )
{
	m_materials.emplace_back( mateiral );
}

StaticMesh::StaticMesh( )
{
	m_renderData = new StaticMeshRenderData( );
}

StaticMesh::StaticMesh( MeshDescription&& meshDescription, std::vector<Material>&& materials )
{
	 m_renderData = new StaticMeshRenderData( );
	 // TODO: 추후작업이 필요함
	 /*m_materials = std::move( materials );

	 m_renderData->AddLODResource( meshDescription, m_materials );

	 ENQUEUE_THREAD_TASK<ThreadType::RenderThread>( [this]( ){
		 m_renderData->InitRenderResource( );
	 } );*/
}

 StaticMesh::~StaticMesh( )
 {
	 // TODO: 추후작업으로 삭제 말고 레퍼 카운트 감소해야 함.
	 delete m_renderData;
 }
