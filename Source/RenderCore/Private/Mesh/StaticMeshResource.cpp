#include "Mesh/StaticMeshResource.h"

#include "AccelerationStructure.h"
#include "Archive.h"
#include "ArchiveUtility.h"
#include "TaskScheduler.h"

#include <cassert>
#include <vector>

namespace rendercore
{
	Archive& operator<<( Archive& ar, StaticMeshSection& section )
	{
		ar << section.m_startLocation
			<< section.m_count
			<< section.m_materialIndex
			<< section.m_meshlets;

		return ar;
	}

	Archive& operator<<( Archive& ar, StaticMeshLODResource& lodResource )
	{
		ar << lodResource.m_vertexCollection;

		if ( ar.IsWriteMode() )
		{
			ar << static_cast<uint32>( lodResource.m_indexData.size() );
		}
		else
		{
			uint32 size = 0;
			ar << size;
			lodResource.m_indexData.resize( size );
		}

		ar << lodResource.m_isDWORD;
		if ( lodResource.m_isDWORD )
		{
			for ( size_t i = 0; i < lodResource.m_indexData.size(); i += sizeof( DWORD ) )
			{
				ar << *reinterpret_cast<DWORD*>( lodResource.m_indexData.data() + i );
			}
		}
		else
		{
			for ( size_t i = 0; i < lodResource.m_indexData.size(); i += sizeof( WORD ) )
			{
				ar << *reinterpret_cast<WORD*>( lodResource.m_indexData.data() + i );
			}
		}

		ar << lodResource.m_sections;
		ar << lodResource.m_meshletVertices;
		ar << lodResource.m_meshletTriangles;

		return ar;
	}

	void StaticMeshRenderData::AllocateLODResources( uint32 numLOD )
	{
		m_lodResources.resize( numLOD );
	}

	void StaticMeshRenderData::Init()
	{
		assert( IsInRenderThread() );
	}

	RefHandle<agl::BLAS> StaticMeshRenderData::GetBLAS() const
	{
		return m_blas;
	}

	void StaticMeshRenderData::CreateRenderResource()
	{
		if ( Initialized() )
		{
			return;
		}

		bool bSupportsMeshShader = GetInterface<agl::IAgl>()->SupportsMeshShader();

		for ( StaticMeshLODResource& lodResource : m_lodResources )
		{
			lodResource.m_vertexCollection.InitResource();

			uint32 indexBufferStride = lodResource.m_isDWORD ? sizeof( DWORD ) : sizeof( WORD );
			auto numIndexBufferElement = static_cast<uint32>( lodResource.m_indexData.size() ) / indexBufferStride;
			lodResource.m_ib = IndexBuffer( numIndexBufferElement, agl::ResourceState::Common, lodResource.m_indexData.data(), lodResource.m_isDWORD );

			if ( bSupportsMeshShader )
			{
				auto numMeshletVertexBufferElement = static_cast<uint32>( lodResource.m_meshletVertices.size() );
				std::construct_at( &lodResource.m_meshletVertexBuffer, numMeshletVertexBufferElement, lodResource.m_meshletVertices.data() );

				auto meshletTriangleBufferElement = static_cast<uint32>( lodResource.m_meshletTriangles.size() );
				std::construct_at( &lodResource.m_meshletTriangleBuffer, meshletTriangleBufferElement, lodResource.m_meshletTriangles.data() );

				for ( StaticMeshSection& section : lodResource.m_sections )
				{
					auto numMeshletElement = static_cast<uint32>( section.m_meshlets.size() );
					std::construct_at( &section.m_meshletBuffer, numMeshletElement, section.m_meshlets.data() );
				}
			}
		}

		if ( m_lodResources.empty() == false )
		{
			StaticMeshLODResource& lodResource = m_lodResources[0];

			VertexBuffer* vertexBuffer = lodResource.m_vertexCollection.GetVertexBuffer( StaticName( "POSITION" ) );
			assert( vertexBuffer );

			if ( GetInterface<agl::IAgl>()->SupportsHardwareRayTracing() )
			{
				agl::BLASDesc desc = {
					.m_vertexBuffer = vertexBuffer->Resource(),
					.m_indexBuffer = lodResource.m_ib.Resource()
				};

				m_blas = agl::BLAS::Create( desc, "StaticMesh.BLAS" );
				EnqueueRenderTask(
					[blas = m_blas]()
					{
						blas->Init();
					} );
			}
		}

		m_initialized = true;
	}

	Archive& operator<<( Archive& ar, StaticMeshRenderData& renderData )
	{
		ar << renderData.m_lodResources;

		return ar;
	}
}
