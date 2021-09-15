#include "stdafx.h"
#include "DrawSnapshot.h"

#include "AbstractGraphicsInterface.h"

#include <optional>

PrimitiveIdVertexBufferPool& PrimitiveIdVertexBufferPool::GetInstance( )
{
	static PrimitiveIdVertexBufferPool primitiveIdVertexBufferPool;
	return primitiveIdVertexBufferPool;
}

VertexBuffer PrimitiveIdVertexBufferPool::Alloc( uint32 require )
{
	constexpr uint32 minimum = 1024;
	require = ( require + minimum - 1 ) & ~( minimum - 1 );

	std::optional<size_t> bestMatchIdx;
	for ( size_t i = 0; i < m_entries.size( ); ++i )
	{
		if ( ( bestMatchIdx.has_value( ) == false ) ||
			( m_entries[i].m_vertexBuffer.Size( ) < m_entries[*bestMatchIdx].m_vertexBuffer.Size() ) )
		{
			bestMatchIdx = i;

			if ( m_entries[i].m_vertexBuffer.Size( ) == require )
			{
				break;
			}
		}
	}

	if ( bestMatchIdx.has_value( ) )
	{
		m_entries[*bestMatchIdx].m_lastDiscardId = m_discardId;
		return m_entries[*bestMatchIdx].m_vertexBuffer;
	}
	else
	{
		m_entries.emplace_back( );
		auto& newEntry = m_entries.back( );
		newEntry.m_lastDiscardId = m_discardId;

		constexpr uint32 elementSize = sizeof( uint32 );
		uint32 numElement = require / sizeof( uint32 );
		newEntry.m_vertexBuffer = VertexBuffer( elementSize, numElement, nullptr, true );

		return newEntry.m_vertexBuffer;
	}
}

void PrimitiveIdVertexBufferPool::DiscardAll( )
{
	++m_discardId;

	for ( auto iter = std::begin( m_entries ); iter != std::end( m_entries ); )
	{
		if ( m_discardId - iter->m_lastDiscardId > 1000u )
		{
			iter = m_entries.erase( iter );
		}
		else
		{
			++iter;
		}
	}
}

void PreparePipelineStateObject( DrawSnapshot& snapshot )
{
	auto& pipelineState = snapshot.m_pipelineState;
	auto& shaderState = pipelineState.m_shaderState;

	aga::PipelineStateInitializer initializer
	{
		shaderState.m_vertexShader ? shaderState.m_vertexShader->Resource( ) : nullptr,
		shaderState.m_pixelShader ? shaderState.m_pixelShader->Resource( ) : nullptr,
		pipelineState.m_blendState.Resource( ),
		pipelineState.m_rasterizerState.Resource( ),
		pipelineState.m_depthStencilState.Resource( ),
		shaderState.m_vertexLayout.Resource( ),
		pipelineState.m_primitive,
	};

	pipelineState.m_pso = aga::PipelineState::Create( initializer );
}

void SortDrawSnapshots( std::vector<VisibleDrawSnapshot>& snapshots, VertexBuffer& primitiveIds )
{
	std::sort( std::begin( snapshots ), std::end( snapshots ), 
		[]( const VisibleDrawSnapshot& lhs, const VisibleDrawSnapshot& rhs )
		{
			return lhs.m_snapshotBucketId < rhs.m_snapshotBucketId;
		} );

	for ( size_t cur = 0, dest = cur + 1; cur < snapshots.size( ) && dest < snapshots.size( ); ++dest )
	{
		if ( snapshots[cur].m_snapshotBucketId != -1 &&
			snapshots[cur].m_snapshotBucketId == snapshots[dest].m_snapshotBucketId )
		{
			++snapshots[cur].m_numInstance;
		}
		else
		{
			cur = dest;
		}
	}

	uint32* idBuffer = reinterpret_cast<uint32*>( primitiveIds.Lock( ) );
	if ( idBuffer )
	{
		for ( size_t i = 0; i < snapshots.size(); ++i )
		{
			snapshots[i].m_primitiveIdOffset = static_cast<uint32>( i );
			*idBuffer = snapshots[i].m_primitiveId;
			++idBuffer;
		}

		primitiveIds.Unlock( );
	}
}

void CommitDrawSnapshots( std::vector<VisibleDrawSnapshot>& snapshots, VertexBuffer& primitiveIds )
{
	for ( size_t i = 0; i < snapshots.size( ); )
	{
		CommitDrawSnapshot( snapshots[i], primitiveIds );
		i += snapshots[i].m_numInstance;
	}
}

void CommitDrawSnapshot( VisibleDrawSnapshot& visibleSnapshot, VertexBuffer& primitiveIds )
{
	DrawSnapshot& snapshot = *visibleSnapshot.m_drawSnapshot;
	auto commandList = GetInterface<aga::IAga>( )->GetImmediateCommandList( );

	// Set vertex buffer
	VertexInputStream vertexStream = snapshot.m_vertexStream;
	vertexStream.Bind( primitiveIds, 1, visibleSnapshot.m_primitiveIdOffset * sizeof( uint32 ) );

	uint32 numVB = vertexStream.NumBuffer( );
	aga::Buffer* const* vertexBuffers = vertexStream.VertexBuffers( );
	const uint32* vertexOffsets = vertexStream.Offsets( );
	commandList->BindVertexBuffer( vertexBuffers, 0, numVB, vertexOffsets );

	// Set index buffer
	aga::Buffer* indexBuffer = snapshot.m_indexBuffer.Resource( );
	uint32 indexOffset = 0;

	commandList->BindIndexBuffer( indexBuffer, indexOffset );

	// Set pipeline state
	commandList->BindPipelineState( snapshot.m_pipelineState.m_pso );

	// Set shader resources
	commandList->BindShaderResources( snapshot.m_shaderBindings );

	if ( visibleSnapshot.m_numInstance > 1 )
	{
		commandList->DrawInstancing( snapshot.m_indexCount, visibleSnapshot.m_numInstance, snapshot.m_startIndexLocation, snapshot.m_baseVertexLocation );
	}
	else
	{
		commandList->Draw( snapshot.m_indexCount, snapshot.m_startIndexLocation, snapshot.m_baseVertexLocation );
	}
}

int32 CachedDrawSnapshotBucket::Add( const DrawSnapshot& snapshot )
{
	constexpr size_t dummy = 0;
	auto [iter, success] = m_bucket.emplace( snapshot, dummy );
	if ( success )
	{
		size_t id = m_snapshots.Add( snapshot );
		iter->second = id;
		return static_cast<int32>( id );
	}

	return static_cast<int32>( iter->second );
}

void CachedDrawSnapshotBucket::Remove( int32 id )
{
	size_t index = static_cast<size_t>( id );
	const DrawSnapshot& snapshot = m_snapshots[index];
	auto found = m_bucket.find( snapshot );
	if ( found == std::end( m_bucket ) )
	{
		// Error
		assert( false );
		return;
	}

	m_bucket.erase( found );
	m_snapshots.RemoveAt( index );
}
