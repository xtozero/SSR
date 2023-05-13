#include "VertexCollection.h"

#include "ArchiveUtility.h"
#include "MeshDescription.h"
#include "VertexBufferBundle.h"

namespace rendercore
{
	VertexStream::VertexStream( const char* name, agl::ResourceFormat format, uint32 count ) : m_name( name ), m_format( format ), m_count( count )
	{
		m_stride = agl::ResourceFormatSize( format );
		m_data.resize( m_stride * m_count );
	}

	Archive& operator<<( Archive& ar, VertexStream& stream )
	{
		ar << stream.m_name
			<< stream.m_format
			<< stream.m_stride
			<< stream.m_count
			<< stream.m_data;

		return ar;
	}

	void VertexStreamLayout::AddLayout( const char* name, uint32 index, agl::ResourceFormat format, uint32 slot, bool isInstanceData, uint32 instanceDataStep, int32 streamIndex )
	{
		m_streamIndices[m_size] = streamIndex;
		VertexLayoutDesc::AddLayout( name, index, format, slot, isInstanceData, instanceDataStep );
	}

	Archive& operator<<( Archive& ar, VertexStreamLayout& layout )
	{
		ar << static_cast<VertexLayoutDesc&>( layout );

		for ( uint32 i = 0; i < layout.Size(); ++i )
		{
			ar << layout.m_streamIndices[i];
		}

		return ar;
	}

	void VertexCollection::InitResource()
	{
		m_vbs.resize( m_streams.size() );

		uint32 i = 0;
		for ( const auto& stream : m_streams )
		{
			m_vbs[i++] = VertexBuffer( stream.Stride(), stream.Count(), stream.Data() );
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
		return static_cast<uint32>( m_streams.size() );
	}

	void VertexCollection::InitLayout( const agl::VertexLayoutTrait* trait, uint32 count, VertexStreamLayoutType layoutType )
	{
		if ( layoutType == VertexStreamLayoutType::PositionOnly )
		{
			m_positionLayout = SetupVertexLayout( trait, count );
		}
		else if ( layoutType == VertexStreamLayoutType::PositionNormal )
		{
			m_positionNormalLayout = SetupVertexLayout( trait, count );
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
		else if ( layoutType == VertexStreamLayoutType::PositionNormal )
		{
			return m_positionNormalLayout;
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
		else if ( layoutType == VertexStreamLayoutType::PositionNormal )
		{
			layout = &m_positionNormalLayout;
		}
		else
		{
			layout = &m_defaultLayout;
		}

		for ( uint32 i = 0; i < layout->Size(); ++i )
		{
			int32 streamIndex = layout->StreamIndex( i );
			uint32 slot = layout->Data()[i].m_slot;

			if ( streamIndex != -1 )
			{
				bundle.Bind( m_vbs[streamIndex], slot );
			}
		}
	}

	Archive& operator<<( Archive& ar, VertexCollection& collection )
	{
		ar << collection.m_streams
			<< collection.m_positionLayout
			<< collection.m_positionNormalLayout
			<< collection.m_defaultLayout;

		return ar;
	}

	std::optional<uint32> VertexCollection::FindStream( const Name& name ) const
	{
		for ( uint32 i = 0; i < m_streams.size(); ++i )
		{
			if ( m_streams[i].GetName() == name )
			{
				return i;
			}
		}

		return { };
	}

	VertexStreamLayout VertexCollection::SetupVertexLayout( const agl::VertexLayoutTrait* traits, uint32 count )
	{
		VertexStreamLayout layout;

		for ( uint32 i = 0; i < count; ++i )
		{
			const agl::VertexLayoutTrait& trait = traits[i];
			auto streamIndex = FindStream( trait.m_name );
			if ( streamIndex )
			{
				layout.AddLayout( trait.m_name.Str().data(),
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

		const std::vector<Vector>& pos = desc.m_positions;
		const std::vector<Vector>& normal = desc.m_normals;
		const std::vector<Vector2>& texCoord = desc.m_texCoords;
		const std::vector<MeshVertexInstance>& vertexInstances = desc.m_vertexInstances;
		uint32 vertexInstanceCount = static_cast<uint32>( vertexInstances.size() );

		agl::VertexLayoutTrait trait[agl::MAX_VERTEX_LAYOUT_SIZE] = {};
		uint32 traitSize = 0;
		uint32 slot = 0;

		// Only Position
		{
			VertexStream posStream( "POSITION", agl::ResourceFormat::R32G32B32_FLOAT, vertexInstanceCount );

			auto data = reinterpret_cast<Vector*>( posStream.Data() );
			for ( size_t i = 0; i < vertexInstances.size(); ++i )
			{
				const MeshVertexInstance& instance = vertexInstances[i];
				data[i] = pos[instance.m_positionID];
			}

			collection.AddStream( std::move( posStream ) );

			trait[traitSize++] = {
				.m_isInstanceData = false,
				.m_index = 0,
				.m_format = agl::ResourceFormat::R32G32B32_FLOAT,
				.m_slot = slot++,
				.m_instanceDataStep = 0,
				.m_name = Name( "POSITION" )
			};

			collection.InitLayout( trait, traitSize, VertexStreamLayoutType::PositionOnly );
		}

		// Normal
		{
			if ( normal.size() > 0 )
			{
				VertexStream normalStream( "NORMAL", agl::ResourceFormat::R32G32B32_FLOAT, vertexInstanceCount );

				auto data = reinterpret_cast<Vector*>( normalStream.Data() );
				for ( size_t i = 0; i < vertexInstances.size(); ++i )
				{
					const MeshVertexInstance& instance = vertexInstances[i];
					data[i] = normal[instance.m_normalID];
				}

				collection.AddStream( std::move( normalStream ) );

				trait[traitSize++] = {
					.m_isInstanceData = false,
					.m_index = 0,
					.m_format = agl::ResourceFormat::R32G32B32_FLOAT,
					.m_slot = slot++,
					.m_instanceDataStep = 0,
					.m_name = Name( "NORMAL" )
				};

				collection.InitLayout( trait, traitSize, VertexStreamLayoutType::PositionNormal );
			}
		}

		// Default
		{

			if ( texCoord.size() > 0 )
			{
				VertexStream texCoordStream( "TEXCOORD", agl::ResourceFormat::R32G32_FLOAT, vertexInstanceCount );

				auto data = reinterpret_cast<Vector2*>( texCoordStream.Data() );
				for ( size_t i = 0; i < vertexInstances.size(); ++i )
				{
					const MeshVertexInstance& instance = vertexInstances[i];
					data[i] = texCoord[instance.m_texCoordID];
				}

				collection.AddStream( std::move( texCoordStream ) );

				trait[traitSize++] = {
					.m_isInstanceData = false,
					.m_index = 0,
					.m_format = agl::ResourceFormat::R32G32_FLOAT,
					.m_slot = slot++,
					.m_instanceDataStep = 0,
					.m_name = Name( "TEXCOORD" )
				};
			}

			collection.InitLayout( trait, traitSize, VertexStreamLayoutType::Default );
		}

		return collection;
	}
}
