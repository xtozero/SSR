#include "stdafx.h"
#include "Mesh/StaticMesh.h"

#include "common.h"
#include "CommonMeshDefine.h"
#include "Material/Material.h"
#include "Material/MaterialResource.h"
#include "MeshDescription.h"
#include "StaticMeshResource.h"
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

	if ( m_renderData == nullptr )
	{
		m_renderData = new StaticMeshRenderData( );
	}
	m_renderData->Serialize( ar );

	if ( ar.IsWriteMode( ) )
	{
		ar << static_cast<uint32>( m_materials.size( ) );
	}
	else
	{
		uint32 size = 0;
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
	delete m_renderData;
	m_renderData = new StaticMeshRenderData( );
	m_renderData->AllocateLODResources( static_cast<uint32>( meshDescriptions.size( ) ) );

	uint32 lodIndex = 0;
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

	size_t normalCount = normal.size( );
	size_t texcoordCount = texCoord.size( );

	for ( size_t i = 0; i < vertexInstances.size( ); ++i )
	{
		const MeshVertexInstance& vertexInstance = vertexInstances[i];

		StaticMeshVertex& v = vertices.emplace_back( );
		v.m_position = pos[vertexInstance.m_positionID];
		v.m_normal = ( vertexInstance.m_normalID < normalCount ) ? normal[vertexInstance.m_normalID] : CXMFLOAT3( 0.f, 0.f, 0.f );
		v.m_texcoord = ( vertexInstance.m_texCoordID < texcoordCount ) ? texCoord[vertexInstance.m_texCoordID] : CXMFLOAT2( 0.f, 0.f );
	}

	const std::vector<MeshTriangle>& triangles = meshDescription.m_triangles;
	const std::vector<MeshPolygon>& polygons = meshDescription.m_polygons;

	size_t indexCount = 0;
	for ( const MeshPolygon& polygon : polygons )
	{
		indexCount += polygon.m_triangleID.size( ) * 3;
	}

	std::vector<size_t> indices;
	indices.reserve( indexCount );

	for ( size_t i = 0; i < polygons.size( ); ++i )
	{
		const MeshPolygon& polygon = polygons[i];

		StaticMeshSection& section = lodResource.m_sections.emplace_back( );
		section.m_startLocation = static_cast<uint32>( indices.size( ) );

		for ( size_t triangleID : polygon.m_triangleID )
		{
			for ( size_t vertexInstanceID : triangles[triangleID].m_vertexInstanceID )
			{
				indices.push_back( vertexInstanceID );
			}
		}

		section.m_count = static_cast<uint32>( indices.size( ) ) - section.m_startLocation;

		const std::string& polygonMaterialName = meshDescription.m_polygonMaterialName[i];
		for ( uint32 j = 0; j < static_cast<uint32>( m_materials.size( ) ); ++j )
		{
			const auto material = m_materials[j].m_mateiral;
			if ( polygonMaterialName == material->Path().stem() )
			{
				section.m_materialIndex = j;
				break;
			}
		}
	}

	lodResource.m_isDWORD = ( vertexInstances.size( ) > std::numeric_limits<DWORD>::max( ) );

	size_t indexStride = lodResource.m_isDWORD ? sizeof( DWORD ) : sizeof( WORD );
	lodResource.m_indexData.resize( indices.size() * indexStride );

	if ( lodResource.m_isDWORD )
	{
		for ( size_t i = 0; i < indices.size( ); ++i )
		{
			reinterpret_cast<DWORD*>( lodResource.m_indexData.data( ) )[i] = static_cast<DWORD>( indices[i] );
		}
	}
	else
	{
		for ( size_t i = 0; i < indices.size( ); ++i )
		{
			reinterpret_cast<WORD*>( lodResource.m_indexData.data( ) )[i] = static_cast<WORD>( indices[i] );
		}
	}
}

MaterialResource* StaticMesh::GetMaterialResource( size_t idx ) const
{
	assert( idx < m_materials.size( ) );
	if ( m_materials[idx].m_mateiral == nullptr )
	{
		return nullptr;
	}

	return m_materials[idx].m_mateiral->GetMaterialResource();
}

void StaticMesh::AddMaterial( const std::shared_ptr<Material>& mateiral )
{
	m_materials.emplace_back( mateiral );
}

StaticMesh::~StaticMesh( )
{
	delete m_renderData;
}

void StaticMesh::PostLoadImpl( )
{
	EnqueueRenderTask( [this]( ) {
		m_renderData->Init( );
	} );
}
