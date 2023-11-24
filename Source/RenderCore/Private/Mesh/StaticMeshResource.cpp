#include "stdafx.h"
#include "Mesh/StaticMeshResource.h"

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
			<< section.m_materialIndex;

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

	void StaticMeshRenderData::CreateRenderResource()
	{
		if ( Initialized() )
		{
			return;
		}

		for ( StaticMeshLODResource& lodResource : m_lodResources )
		{
			lodResource.m_vertexCollection.InitResource();

			uint32 stride = lodResource.m_isDWORD ? sizeof( DWORD ) : sizeof( WORD );
			uint32 size = static_cast<uint32>( lodResource.m_indexData.size() ) / stride;
			lodResource.m_ib = IndexBuffer( size, agl::ResourceState::Common, lodResource.m_indexData.data(), lodResource.m_isDWORD );
		}

		m_initialized = true;
	}

	Archive& operator<<( Archive& ar, StaticMeshRenderData& renderData )
	{
		ar << renderData.m_lodResources;

		return ar;
	}
}
