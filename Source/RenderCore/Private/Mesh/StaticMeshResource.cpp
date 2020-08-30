#include "stdafx.h"
#include "Mesh/StaticMeshResource.h"

#include "../../Logic/Public/Material/Material.h"
#include "../../Logic/Public/Model/MeshDescription.h"
#include "Math/CXMFloat.h"

#include <vector>

StaticMeshVertexLayout::StaticMeshVertexLayout( const char* name, int index, RESOURCE_FORMAT format, int slot, bool isInstanceData, int instanceDataStep ) : VERTEX_LAYOUT { name, index, format, slot, isInstanceData, instanceDataStep }
{
}

void StaticMeshRenderData::AddLODResource( const MeshDescription& meshDescription, const std::vector<Material>& materials )
{
	StaticMeshLODResource& lodResource = m_lodResources.emplace_back( );

	const std::vector<CXMFLOAT3>& pos = meshDescription.m_positions;
	const std::vector<CXMFLOAT3>& normal = meshDescription.m_normals;
	const std::vector<CXMFLOAT2>& texCoord = meshDescription.m_texCoords;
	const std::vector<MeshVertexInstance>& vertexInstances = meshDescription.m_vertexInstances;

	std::vector<StaticMeshVertex> vertices;
	vertices.reserve( vertexInstances.size( ) );

	std::size_t normalCount = normal.size( );
	std::size_t texcoordCount = texCoord.size( );

	for ( std::size_t i = 0; i < vertexInstances.size( ); ++i )
	{
		const MeshVertexInstance& vertexInstance = vertexInstances[i];

		StaticMeshVertex& v = vertices.emplace_back( );
		v.m_posision = pos[vertexInstance.m_positionID];
		v.m_normal = ( vertexInstance.m_normalID < normalCount ) ? normal[vertexInstance.m_normalID] : CXMFLOAT3( 0.f, 0.f, 0.f );
		v.m_texcoord = ( vertexInstance.m_texCoordID < texcoordCount ) ? texCoord[vertexInstance.m_texCoordID] : CXMFLOAT2( 0.f, 0.f );
	}

	lodResource.m_vb.Initialize( std::move( vertices ) );

	const std::vector<MeshTriangle>& triangles = meshDescription.m_triangles;
	const std::vector<MeshPolygon>& polygons = meshDescription.m_polygons;

	std::size_t indexCount = 0;
	for ( const MeshPolygon& polygon : polygons )
	{
		indexCount += polygon.m_triangleID.size( ) * 3;
	}

	std::vector<std::size_t> indices;
	indices.reserve( indexCount );

	for ( std::size_t i = 0; i < polygons.size(); ++i )
	{
		const MeshPolygon& polygon = polygons[i];

		StaticMeshSection& section = m_sections.emplace_back( );
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
		for ( std::size_t j = 0; j < materials.size(); ++j )
		{
			if ( polygonMaterialName == materials[j].Name( ) )
			{
				section.m_materialIndex = j;
				break;
			}
		}
	}

	lodResource.m_ib.Initialize( indices );
}

void StaticMeshRenderData::PostLODResourceSetup( )
{
	m_vertexLayouts.reserve( 3 );

	// position
	m_vertexLayouts.emplace_back(
			"POSITION",
			0,
			RESOURCE_FORMAT::R32G32B32_FLOAT,
			0,
			false,
			0
	);

	// normal
	m_vertexLayouts.emplace_back(
			"NORMAL",
			0,
			RESOURCE_FORMAT::R32G32B32_FLOAT,
			0,
			false,
			0
	);

	// texcoord
	m_vertexLayouts.emplace_back(
			"TEXCOORD",
			0,
			RESOURCE_FORMAT::R32G32_FLOAT,
			0,
			false,
			0
	);
}
