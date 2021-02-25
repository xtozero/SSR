#include "stdafx.h"
#include "Mesh/StaticMeshResource.h"

#include "../../Logic/Public/Material/Material.h"
#include "../../Logic/Public/Model/MeshDescription.h"
#include "Archive.h"
#include "Math/CXMFloat.h"
#include "MultiThread/EngineTaskScheduler.h"

#include <cassert>
#include <vector>

void StaticMeshVertex::Serialize( Archive& ar )
{
	ar << m_position.x << m_position.y << m_position.z
		<< m_normal.x << m_normal.y << m_normal.z
		<< m_texcoord.x << m_texcoord.y;
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
	if ( ar.IsWriteMode( ) )
	{
		ar << m_vertexData.size( );
	}
	else
	{
		std::size_t size = 0;
		ar << size;
		m_vertexData.resize( size );
	}

	for ( auto& vertex : m_vertexData )
	{
		ar << vertex;
	}

	if ( ar.IsWriteMode( ) )
	{
		ar << m_indexData.size( );
	}
	else
	{
		std::size_t size = 0;
		ar << size;
		m_indexData.resize( size );
	}

	for ( auto& index : m_indexData )
	{
		ar << index;
	}

	if ( ar.IsWriteMode( ) )
	{
		ar << m_sections.size( );
	}
	else
	{
		std::size_t size = 0;
		ar << size;
		m_sections.resize( size );
	}

	for ( auto& section : m_sections )
	{
		ar << section;
	}
}

Archive& operator<<( Archive& ar, StaticMeshLODResource& lodResource )
{
	lodResource.Serialize( ar );
	return ar;
}

void StaticMeshRenderData::AllocateLODResources( std::size_t numLOD )
{
	m_lodResources.resize( numLOD );
	m_vertexLayouts.resize( numLOD );
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
		lodResource.m_vb = TypedVertexBuffer<StaticMeshVertex>::Create( vertexData.size( ), vertexData.data( ) );

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

void StaticMeshRenderData::Serialize( Archive& ar )
{
	if ( ar.IsWriteMode( ) )
	{
		ar << m_lodResources.size( );
	}
	else
	{
		std::size_t size = 0;
		ar << size;
		m_lodResources.resize( size );
	}

	for ( auto& lodResource : m_lodResources )
	{
		ar << lodResource;
	}
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
