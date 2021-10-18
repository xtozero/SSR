#include "VertexCollection.h"

#include "ArchiveUtility.h"
#include "Math/CXMFloat.h"
#include "MeshDescription.h"
#include "VertexBufferBundle.h"

Archive& operator<<( Archive& ar, VertexStream& stream )
{
	stream.Serialize( ar );
	return ar;
}

Archive& operator<<( Archive& ar, VertexStreamLayout& inputLayout )
{
	inputLayout.Serialize( ar );
	return ar;
}

Archive& operator<<( Archive& ar, VertexCollection& vertexCollection )
{
	vertexCollection.Serialize( ar );
	return ar;
}

void VertexStream::Serialize( Archive& ar )
{
	ar << m_name << m_format << m_stride << m_count << m_data;
}

VertexStream::VertexStream( const char* name, RESOURCE_FORMAT format, uint32 count ) : m_name( name ), m_format( format ), m_count( count )
{
	m_stride = ResourceFormatSize( format );
	m_data.resize( m_stride * m_count );
}

void VertexStreamLayout::Serialize( Archive& ar )
{
	VertexLayoutDesc::Serialize( ar );

	for ( uint32 i = 0; i < Size( ); ++i )
	{
		ar << m_streamIndices[i];
	}
}

void VertexStreamLayout::AddLayout( const char* name, uint32 index, RESOURCE_FORMAT format, uint32 slot, bool isInstanceData, uint32 instanceDataStep, int32 streamIndex )
{
	m_streamIndices[m_size] = streamIndex;
	VertexLayoutDesc::AddLayout( name, index, format, slot, isInstanceData, instanceDataStep );
}

void VertexCollection::Serialize( Archive& ar )
{
	ar << m_streams;
	ar << m_defaultLayout;
	ar << m_positionLayout;
}

void VertexCollection::InitResource( )
{
	m_vbs.resize( m_streams.size( ) );

	uint32 i = 0;
	for ( const auto& stream : m_streams )
	{
		m_vbs[i++] = VertexBuffer( stream.Stride( ), stream.Count( ), stream.Data( ) );
	}
}

uint32 VertexCollection::AddStream( const VertexStream& stream )
{
	m_streams.emplace_back( stream );
	return static_cast<uint32>( m_streams.size() );
}

uint32 VertexCollection::AddStream( VertexStream&& stream )
{
	m_streams.emplace_back( std::move( stream ) );
	return static_cast<uint32>( m_streams.size( ) );
}

void VertexCollection::InitLayout( const VERTEX_LAYOUT_TRAIT* trait, uint32 count, VertexStreamLayoutType layoutType )
{
	if ( layoutType == VertexStreamLayoutType::PositionOnly )
	{
		m_positionLayout = SetupVertexLayout( trait, count );
	}
	else
	{
		m_defaultLayout = SetupVertexLayout( trait, count );
	}
}

const VertexStreamLayout& VertexCollection::VertexLayout( VertexStreamLayoutType layoutType ) const
{
	if ( layoutType == VertexStreamLayoutType::PositionOnly )
	{
		return m_positionLayout;
	}

	return m_defaultLayout;
}

void VertexCollection::Bind( VertexBufferBundle& bundle, VertexStreamLayoutType layoutType ) const
{
	const VertexStreamLayout* layout;
	if ( layoutType == VertexStreamLayoutType::PositionOnly )
	{
		layout = &m_positionLayout;
	}
	else
	{
		layout = &m_defaultLayout;
	}

	for ( uint32 i = 0; i < layout->Size( ); ++i )
	{
		int32 streamIndex = layout->StreamIndex( i );
		uint32 slot = layout->Data( )[i].m_slot;

		if ( streamIndex != -1 )
		{
			bundle.Bind( m_vbs[streamIndex], slot );
		}
	}
}

std::optional<uint32> VertexCollection::FindStream( const std::string& name ) const
{
	for ( uint32 i = 0; i < m_streams.size( ); ++i )
	{
		if ( m_streams[i].Name( ) == name )
		{
			return i;
		}
	}

	return { };
}

VertexStreamLayout VertexCollection::SetupVertexLayout( const VERTEX_LAYOUT_TRAIT* traits, uint32 count )
{
	VertexStreamLayout layout;

	for ( uint32 i = 0; i < count; ++i )
	{
		const VERTEX_LAYOUT_TRAIT& trait = traits[i];
		auto streamIndex = FindStream( trait.m_name );
		if ( streamIndex )
		{
			layout.AddLayout( trait.m_name.c_str( ),
							trait.m_index,
							trait.m_format,
							trait.m_slot,
							trait.m_isInstanceData,
							trait.m_instanceDataStep,
							*streamIndex );
		}
	}

	return layout;
}

VertexCollection BuildFromMeshDescription( const MeshDescription& desc )
{
	VertexCollection collection;

	const std::vector<CXMFLOAT3>& pos = desc.m_positions;
	const std::vector<CXMFLOAT3>& normal = desc.m_normals;
	const std::vector<CXMFLOAT2>& texCoord = desc.m_texCoords;
	const std::vector<MeshVertexInstance>& vertexInstances = desc.m_vertexInstances;
	uint32 vertexInstanceCount = static_cast<uint32>( vertexInstances.size( ) );

	VERTEX_LAYOUT_TRAIT trait[MAX_VERTEX_LAYOUT_SIZE] = {};
	uint32 traitSize = 0;
	uint32 slot = 0;

	// Only Position
	{
		VertexStream posStream( "POSITION", RESOURCE_FORMAT::R32G32B32_FLOAT, vertexInstanceCount );

		CXMFLOAT3* data = reinterpret_cast<CXMFLOAT3*>( posStream.Data( ) );
		for ( size_t i = 0; i < vertexInstances.size( ); ++i )
		{
			const MeshVertexInstance& instance = vertexInstances[i];
			data[i] = pos[instance.m_positionID];
		}

		collection.AddStream( std::move( posStream ) );

		trait[traitSize++] = {
			false,
			0,
			RESOURCE_FORMAT::R32G32B32_FLOAT,
			slot++,
			0,
			"POSITION"
		};

		collection.InitLayout( trait, traitSize, VertexStreamLayoutType::PositionOnly );
	}

	// Default
	{
		if ( normal.size( ) > 0 )
		{
			VertexStream normalStream( "NORMAL", RESOURCE_FORMAT::R32G32B32_FLOAT, vertexInstanceCount );

			CXMFLOAT3* data = reinterpret_cast<CXMFLOAT3*>( normalStream.Data( ) );
			for ( size_t i = 0; i < vertexInstances.size( ); ++i )
			{
				const MeshVertexInstance& instance = vertexInstances[i];
				data[i] = normal[instance.m_normalID];
			}

			collection.AddStream( std::move( normalStream ) );

			trait[traitSize++] = {
			false,
			0,
			RESOURCE_FORMAT::R32G32B32_FLOAT,
			slot++,
			0,
			"NORMAL"
			};
		}
		
		if ( texCoord.size( ) > 0 )
		{
			VertexStream texCoordStream( "TEXCOORD", RESOURCE_FORMAT::R32G32_FLOAT, vertexInstanceCount );
			
			CXMFLOAT2* data = reinterpret_cast<CXMFLOAT2*>( texCoordStream.Data( ) );
			for ( size_t i = 0; i < vertexInstances.size( ); ++i )
			{
				const MeshVertexInstance& instance = vertexInstances[i];
				data[i] = texCoord[instance.m_texCoordID];
			}

			collection.AddStream( std::move( texCoordStream ) );

			trait[traitSize++] = {
			false,
			0,
			RESOURCE_FORMAT::R32G32_FLOAT,
			slot++,
			0,
			"TEXCOORD"
			};
		}

		collection.InitLayout( trait, traitSize, VertexStreamLayoutType::Default );
	}
	
	return collection;
}
