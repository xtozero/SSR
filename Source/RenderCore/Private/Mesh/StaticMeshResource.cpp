#include "stdafx.h"
#include "Mesh/StaticMeshResource.h"

#include "Archive.h"
#include "ArchiveUtility.h"
#include "TaskScheduler.h"

#include <cassert>
#include <vector>

void StaticMeshSection::Serialize( Archive& ar )
{
	ar << m_startLocation << m_count << m_materialIndex;
}

Archive& operator<<( Archive& ar, StaticMeshSection& section )
{
	section.Serialize( ar );
	return ar;
}

void StaticMeshLODResource::Serialize( Archive& ar )
{
	ar << m_vertexCollection;

	if ( ar.IsWriteMode( ) )
	{
		ar << static_cast<uint32>( m_indexData.size( ) );
	}
	else
	{
		uint32 size = 0;
		ar << size;
		m_indexData.resize( size );
	}

	ar << m_isDWORD;
	if ( m_isDWORD )
	{
		for ( size_t i = 0; i < m_indexData.size(); i += sizeof( DWORD ) )
		{
			ar << *reinterpret_cast<DWORD*>( m_indexData.data( ) + i );
		}
	}
	else
	{
		for ( size_t i = 0; i < m_indexData.size( ); i += sizeof( WORD ) )
		{
			ar << *reinterpret_cast<WORD*>( m_indexData.data( ) + i );
		}
	}

	ar << m_sections;
}

Archive& operator<<( Archive& ar, StaticMeshLODResource& lodResource )
{
	lodResource.Serialize( ar );
	return ar;
}

void StaticMeshRenderData::AllocateLODResources( uint32 numLOD )
{
	m_lodResources.resize( numLOD );
}

void StaticMeshRenderData::Init( )
{
	assert( IsInRenderThread( ) );
}

void StaticMeshRenderData::Serialize( Archive& ar )
{
	ar << m_lodResources;
}

void StaticMeshRenderData::CreateRenderResource( )
{
	if ( Initialized( ) )
	{
		return;
	}

	for ( StaticMeshLODResource& lodResource : m_lodResources )
	{
		lodResource.m_vertexCollection.InitResource( );

		uint32 stride = lodResource.m_isDWORD ? sizeof( DWORD ) : sizeof( WORD );
		uint32 size = static_cast<uint32>( lodResource.m_indexData.size( ) ) / stride;
		lodResource.m_ib = IndexBuffer( size, lodResource.m_indexData.data( ), lodResource.m_isDWORD );
	}

	m_initialized = true;
}
