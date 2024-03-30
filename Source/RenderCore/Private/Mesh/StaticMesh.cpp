#include "Mesh/StaticMesh.h"

#include "ArchiveUtility.h"
#include "common.h"
#include "Material/Material.h"
#include "Material/MaterialResource.h"
#include "MeshDescription.h"
#include "StaticMeshResource.h"
#include "TaskScheduler.h"
#include "VertexCollection.h"

namespace rendercore
{
	Archive& operator<<( Archive& ar, StaticMeshMaterial& m )
	{
		ar << m.m_mateiral;

		return ar;
	}

	REGISTER_ASSET( StaticMesh );
	void StaticMesh::Serialize( Archive& ar )
	{
		Super::Serialize( ar );

		if ( m_renderData == nullptr )
		{
			m_renderData = new StaticMeshRenderData();
		}
		ar << *m_renderData;
	}

	void StaticMesh::BuildMeshFromMeshDescriptions( const std::vector<MeshDescription>& meshDescriptions )
	{
		delete m_renderData;
		m_renderData = new StaticMeshRenderData();
		m_renderData->AllocateLODResources( static_cast<uint32>( meshDescriptions.size() ) );

		uint32 lodIndex = 0;
		for ( const auto& meshDescription : meshDescriptions )
		{
			StaticMeshLODResource& lodResource = m_renderData->LODResource( lodIndex );
			BuildMeshFromMeshDescription( meshDescription, lodResource );
		}
	}

	void StaticMesh::BuildMeshFromMeshDescription( const MeshDescription& meshDescription, StaticMeshLODResource& lodResource )
	{
		const std::vector<MeshVertexInstance>& vertexInstances = meshDescription.m_vertexInstances;

		lodResource.m_vertexCollection = BuildFromMeshDescription( meshDescription );

		const std::vector<MeshTriangle>& triangles = meshDescription.m_triangles;
		const std::vector<MeshPolygon>& polygons = meshDescription.m_polygons;

		size_t indexCount = 0;
		for ( const MeshPolygon& polygon : polygons )
		{
			indexCount += polygon.m_triangleID.size() * 3;
		}

		std::vector<size_t> indices;
		indices.reserve( indexCount );

		for ( size_t i = 0; i < polygons.size(); ++i )
		{
			const MeshPolygon& polygon = polygons[i];

			StaticMeshSection& section = lodResource.m_sections.emplace_back();
			section.m_startLocation = static_cast<uint32>( indices.size() );

			for ( size_t triangleID : polygon.m_triangleID )
			{
				for ( size_t vertexInstanceID : triangles[triangleID].m_vertexInstanceID )
				{
					indices.push_back( vertexInstanceID );
				}
			}

			section.m_count = static_cast<uint32>( indices.size() ) - section.m_startLocation;

			const Name& polygonMaterialName = meshDescription.m_polygonMaterialName[i];
			for ( uint32 j = 0; j < static_cast<uint32>( m_materials.size() ); ++j )
			{
				const auto material = m_materials[j].m_mateiral;
				if ( polygonMaterialName == material->GetName() )
				{
					section.m_materialIndex = j;
					break;
				}
			}
		}

		lodResource.m_isDWORD = ( vertexInstances.size() > std::numeric_limits<DWORD>::max() );

		size_t indexStride = lodResource.m_isDWORD ? sizeof( DWORD ) : sizeof( WORD );
		lodResource.m_indexData.resize( indices.size() * indexStride );

		if ( lodResource.m_isDWORD )
		{
			for ( size_t i = 0; i < indices.size(); ++i )
			{
				reinterpret_cast<DWORD*>( lodResource.m_indexData.data() )[i] = static_cast<DWORD>( indices[i] );
			}
		}
		else
		{
			for ( size_t i = 0; i < indices.size(); ++i )
			{
				reinterpret_cast<WORD*>( lodResource.m_indexData.data() )[i] = static_cast<WORD>( indices[i] );
			}
		}
	}

	MaterialResource* StaticMesh::GetMaterialResource( size_t idx ) const
	{
		assert( idx < m_materials.size() );
		if ( m_materials[idx].m_mateiral == nullptr )
		{
			return nullptr;
		}

		return m_materials[idx].m_mateiral->GetMaterialResource();
	}

	std::shared_ptr<Material> StaticMesh::GetMaterial( size_t idx ) const
	{
		if ( idx >= m_materials.size() )
		{
			return nullptr;
		}

		return m_materials[idx].m_mateiral;
	}

	void StaticMesh::AddMaterial( const std::shared_ptr<Material>& mateiral )
	{
		m_materials.emplace_back( mateiral );
	}

	uint32 StaticMesh::NumMaterials() const
	{
		return static_cast<uint32>( m_materials.size() );
	}

	StaticMesh::~StaticMesh()
	{
		delete m_renderData;
	}

	void StaticMesh::PostLoadImpl()
	{
		EnqueueRenderTask(
			[this]()
			{
				m_renderData->Init();
			} );
	}
}
