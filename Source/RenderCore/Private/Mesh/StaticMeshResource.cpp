#include "stdafx.h"
#include "Mesh/StaticMeshResource.h"

#include "Archive.h"
#include "ArchiveUtility.h"
#include "Math/CXMFloat.h"
#include "TaskScheduler.h"

#include <cassert>
#include <vector>

void StaticMeshVertex::Serialize( Archive& ar )
{
	ar << m_position << m_normal << m_texcoord;
}

Archive& operator<<( Archive& ar, StaticMeshVertex& vertex )
{
	vertex.Serialize( ar );
	return ar;
}

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
	ar << m_vertexData;

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
	m_vertexLayouts.resize( numLOD );
}

void StaticMeshRenderData::Init( )
{
	assert( IsInRenderThread( ) );

	m_vertexLayouts.resize( m_lodResources.size( ) );

	for ( size_t i = 0; i < m_lodResources.size( ); ++i )
	{
		StaticMeshLODResource& lodResource = m_lodResources[i];
		m_vertexLayouts[i].Initialize( &lodResource );
	}
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
		std::vector<StaticMeshVertex>& vertexData = lodResource.m_vertexData;
		lodResource.m_vb = TypedVertexBuffer<StaticMeshVertex>( static_cast<uint32>( vertexData.size( ) ), vertexData.data( ) );

		uint32 stride = lodResource.m_isDWORD ? sizeof( DWORD ) : sizeof( WORD );
		uint32 size = static_cast<uint32>( lodResource.m_indexData.size( ) ) / stride;
		lodResource.m_ib = IndexBuffer( size, lodResource.m_indexData.data( ), lodResource.m_isDWORD );
	}

	m_initialized = true;
}

void StaticMeshVertexLayout::Initialize( const StaticMeshLODResource* lodResource )
{
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
