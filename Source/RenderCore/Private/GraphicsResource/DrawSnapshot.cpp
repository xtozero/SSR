#include "stdafx.h"
#include "DrawSnapshot.h"

#include "AbstractGraphicsInterface.h"
#include "RenderView.h"
#include "SceneRenderer.h"

#include <optional>
#include <type_traits>

namespace
{
	class CommitDrawSnapshotTask
	{
	public:
		void DoTask( )
		{
			for ( auto drawSnapshot : m_drawSnapshot )
			{
				CommitDrawSnapshot( m_commandList, *drawSnapshot, m_primitiveIds );
			}
			m_commandList.Finish( );
		}

		void AddSnapshot( VisibleDrawSnapshot* snapshot )
		{
			m_drawSnapshot.push_back( snapshot );
		}

		CommitDrawSnapshotTask( size_t reserveSize, aga::IDeferredCommandList& commandList, VertexBuffer& primitiveIds ) : m_commandList( commandList ), m_primitiveIds( primitiveIds )
		{
			m_drawSnapshot.reserve( reserveSize );
		}

	private:
		aga::IDeferredCommandList& m_commandList;
		VertexBuffer& m_primitiveIds;
		std::vector<VisibleDrawSnapshot*> m_drawSnapshot;
	};
}

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
		if ( ( m_entries[i].m_lastDiscardId == m_discardId ) || ( m_entries[i].m_vertexBuffer.Size( ) < require ) )
		{
			continue;
		}

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
		shaderState.m_geometryShader ? shaderState.m_geometryShader->Resource( ) : nullptr,
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

void CommitDrawSnapshots( SceneRenderer& renderer, std::vector<VisibleDrawSnapshot>& visibleSnapshots, VertexBuffer& primitiveIds )
{
	auto commandList = GetInterface<aga::IAga>( )->GetImmediateCommandList( );

	renderer.ApplyOutputContext( *commandList );

	for ( size_t i = 0; i < visibleSnapshots.size( ); )
	{
		CommitDrawSnapshot( *commandList, visibleSnapshots[i], primitiveIds );
		i += visibleSnapshots[i].m_numInstance;
	}
}

void CommitDrawSnapshot( aga::ICommandList& commandList, VisibleDrawSnapshot& visibleSnapshot, VertexBuffer& primitiveIds )
{
	DrawSnapshot& snapshot = *visibleSnapshot.m_drawSnapshot;

	// Set vertex buffer
	VertexBufferBundle vertexStream = snapshot.m_vertexStream;
	if ( snapshot.m_primitiveIdSlot != -1 )
	{
		vertexStream.Bind( primitiveIds, static_cast<uint32>( snapshot.m_primitiveIdSlot ), visibleSnapshot.m_primitiveIdOffset * sizeof( uint32 ) );
	}

	uint32 numVB = vertexStream.NumBuffer( );
	aga::Buffer* const* vertexBuffers = vertexStream.VertexBuffers( );
	const uint32* vertexOffsets = vertexStream.Offsets( );
	commandList.BindVertexBuffer( vertexBuffers, 0, numVB, vertexOffsets );

	// Set index buffer
	aga::Buffer* indexBuffer = snapshot.m_indexBuffer.Resource( );
	commandList.BindIndexBuffer( indexBuffer, 0 );

	// Set pipeline state
	commandList.BindPipelineState( snapshot.m_pipelineState.m_pso );

	// Set shader resources
	commandList.BindShaderResources( snapshot.m_shaderBindings );

	if ( indexBuffer )
	{
		commandList.DrawIndexedInstanced( snapshot.m_count, visibleSnapshot.m_numInstance, snapshot.m_startIndexLocation, snapshot.m_baseVertexLocation );
	}
	else
	{
		commandList.DrawInstanced( snapshot.m_count, visibleSnapshot.m_numInstance, snapshot.m_baseVertexLocation );
	}
}

void ParallelCommitDrawSnapshot( SceneRenderer& renderer, std::vector<VisibleDrawSnapshot>& visibleSnapshots, VertexBuffer& primitiveIds )
{
	size_t dc = 0;
	for ( size_t i = 0; i < visibleSnapshots.size( ); )
	{
		++dc;
		i += visibleSnapshots[i].m_numInstance;
	}

	if ( dc < 64 )
	{
		CommitDrawSnapshots( renderer, visibleSnapshots, primitiveIds );
	}
	else
	{
		auto taskScheduler = GetInterface<ITaskScheduler>( );
		constexpr size_t affinityMask = WorkerAffinityMask<WorkerThreads>( );
		TaskHandle taskGroup = taskScheduler->GetTaskGroup( );

		CommitDrawSnapshotTask* commitTasks[2] = {};
		std::unique_ptr<aga::IDeferredCommandList> deferredCommandLists[2] = {};

		for ( size_t i = 0, j = 0; i < std::extent_v<decltype(commitTasks)>; ++i )
		{
			deferredCommandLists[i] = GetInterface<aga::IAga>( )->CreateDeferredCommandList( );

			size_t count = ( dc + 1 ) / 2;
			auto task = Task<CommitDrawSnapshotTask>::Create( affinityMask, count, *deferredCommandLists[i], primitiveIds );
			commitTasks[i] = &task->Element( );

			size_t added = 0;
			for ( ; j < visibleSnapshots.size( ) && ( added < count ); )
			{
				commitTasks[i]->AddSnapshot( &visibleSnapshots[j] );
				j += visibleSnapshots[j].m_numInstance;
				++added;
			}

			taskGroup.AddTask( task );
		}

		for ( size_t i = 0; i < std::extent_v<decltype( deferredCommandLists )>; ++i )
		{
			renderer.ApplyOutputContext( *deferredCommandLists[i] );
		}

		taskScheduler->Run( taskGroup );
		taskScheduler->Wait( taskGroup );

		auto immediateCommandList = GetInterface<aga::IAga>( )->GetImmediateCommandList( );
		
		for ( size_t i = 0; i < std::extent_v<decltype( deferredCommandLists )>; ++i )
		{
			immediateCommandList->Execute( *deferredCommandLists[i] );
		}
	}
}

int32 CachedDrawSnapshotBucket::Add( const DrawSnapshot& snapshot )
{
	constexpr SharedSnapshotId dummy( 0 );
	auto [iter, success] = m_bucket.emplace( snapshot, dummy );
	if ( success )
	{
		size_t id = m_snapshots.Add( snapshot );
		iter->second.m_id = static_cast<int32>( id );
	}

	++iter->second.m_ref;
	return iter->second.m_id;
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

	--found->second.m_ref;
	if ( found->second.m_ref == 0 )
	{
		m_bucket.erase( found );
		m_snapshots.RemoveAt( index );
	}
}
