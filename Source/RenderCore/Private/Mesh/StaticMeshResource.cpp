#include "stdafx.h"
#include "Mesh/StaticMeshResource.h"

#include "../../Logic/Public/Material/Material.h"
#include "../../Logic/Public/Model/MeshDescription.h"
#include "Math/CXMFloat.h"
#include "MultiThread/EngineTaskScheduler.h"

#include <cassert>
#include <vector>

void StaticMeshRenderData::AddLODResource( const MeshDescription& meshDescription, const std::vector<Material>& materials )
{
	StaticMeshLODResource& lodResource = m_lodResources.emplace_back( );
	m_vertexLayouts.emplace_back( );

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
		v.m_posision = pos[vertexInstance.m_positionID];
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

	for ( std::size_t i = 0; i < polygons.size(); ++i )
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
		for ( std::size_t j = 0; j < materials.size(); ++j )
		{
			if ( polygonMaterialName == materials[j].Name( ) )
			{
				section.m_materialIndex = j;
				break;
			}
		}
	}
}

void StaticMeshRenderData::InitRenderResource( )
{
	assert( IsInRenderThread( ) );

	void* indexData = nullptr;
	std::size_t indexDataByteWidth = 0;

	for ( std::size_t i = 0; i < m_lodResources.size(); ++i )
	{
		StaticMeshLODResource& lodResource = m_lodResources[i];
		m_vertexLayouts[i].Initialize( &lodResource );

		std::vector<StaticMeshVertex>& vertexData = lodResource.m_vertexData;
		lodResource.m_vb.Initialize( vertexData.size( ), vertexData.data( ) );

		bool isDWORD = vertexData.size( ) > std::numeric_limits<WORD>::max( );
		std::size_t requireByteWidth = ( isDWORD ? 4 : 2 ) * lodResource.m_indexData.size( );

		if ( indexDataByteWidth < requireByteWidth )
		{
			delete[] indexData;
			indexData = new unsigned char[requireByteWidth];
			indexDataByteWidth = requireByteWidth;

			if ( isDWORD )
			{
				for ( std::size_t j = 0; j < lodResource.m_indexData.size( ); ++j )
				{
					static_cast<DWORD*>( indexData )[j] = static_cast<DWORD>( lodResource.m_indexData[j] );
				}
			}
			else
			{
				for ( std::size_t j = 0; j < lodResource.m_indexData.size( ); ++j )
				{
					static_cast<WORD*>( indexData )[j] = static_cast<WORD>( lodResource.m_indexData[j] );
				}
			}
		}

		lodResource.m_ib = IndexBuffer::Create( lodResource.m_indexData.size( ), indexData, isDWORD );
	}

	delete[] indexData;
}

void StaticMeshVertexLayout::Initialize( const StaticMeshLODResource* lodResource )
{
	m_vertexLayoutDesc.Initialize( 3 );

	// position
	m_vertexLayoutDesc.AddLayout( "POSITION", 0,
									RESOURCE_FORMAT::R32G32B32_FLOAT,
									0,
									false,
									0 );

	// normal
	m_vertexLayoutDesc.AddLayout( "NORMAL", 0,
									RESOURCE_FORMAT::R32G32B32_FLOAT,
									0,
									false,
									0 );

	// texcoord
	m_vertexLayoutDesc.AddLayout( "TEXCOORD", 0,
									RESOURCE_FORMAT::R32G32_FLOAT,
									0,
									false,
									0 );
}
