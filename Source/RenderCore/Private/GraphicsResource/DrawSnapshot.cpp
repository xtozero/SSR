#include "DrawSnapshot.h"

#include "AbstractGraphicsInterface.h"
#include "CommandList.h"
#include "RenderView.h"
#include "SceneRenderer.h"

#include <optional>
#include <type_traits>

using ::rendercore::VertexBuffer;
using ::rendercore::VisibleDrawSnapshot;

namespace
{
	class CommitDrawSnapshotTask
	{
	public:
		void DoTask()
		{
			for ( auto drawSnapshot : m_drawSnapshot )
			{
				CommitDrawSnapshot( m_commandList, *drawSnapshot, m_primitiveIds );
			}
		}

		void AddSnapshot( VisibleDrawSnapshot* snapshot )
		{
			m_drawSnapshot.push_back( snapshot );
		}

		CommitDrawSnapshotTask( size_t reserveSize, agl::ICommandList& commandList, const VertexBuffer& primitiveIds ) : m_commandList( commandList ), m_primitiveIds( primitiveIds )
		{
			m_drawSnapshot.reserve( reserveSize );
		}

	private:
		agl::ICommandList& m_commandList;
		const VertexBuffer& m_primitiveIds;
		std::vector<VisibleDrawSnapshot*> m_drawSnapshot;
	};
}

namespace rendercore
{
	VertexBuffer PrimitiveIdVertexBufferPool::Alloc( uint32 require )
	{
		constexpr uint32 Minimum = 1024;
		require = CalcAlignment( require, Minimum );

		std::optional<size_t> bestMatchIdx;
		for ( size_t i = 0; i < m_entries.size(); ++i )
		{
			if ( ( m_entries[i].m_lastDiscardId == m_discardId ) || ( m_entries[i].m_vertexBuffer.Size() < require ) )
			{
				continue;
			}

			if ( ( bestMatchIdx.has_value() == false ) ||
				( m_entries[i].m_vertexBuffer.Size() < m_entries[*bestMatchIdx].m_vertexBuffer.Size() ) )
			{
				bestMatchIdx = i;

				if ( m_entries[i].m_vertexBuffer.Size() == require )
				{
					break;
				}
			}
		}

		if ( bestMatchIdx.has_value() )
		{
			m_entries[*bestMatchIdx].m_lastDiscardId = m_discardId;
			return m_entries[*bestMatchIdx].m_vertexBuffer;
		}
		else
		{
			m_entries.emplace_back();
			auto& newEntry = m_entries.back();
			newEntry.m_lastDiscardId = m_discardId;

			constexpr uint32 ElementSize = sizeof( uint32 );
			uint32 numElement = require / sizeof( uint32 );
			newEntry.m_vertexBuffer = VertexBuffer( ElementSize, numElement, agl::ResourceFormat::Unknown, agl::ResourceState::Common, nullptr, true );

			return newEntry.m_vertexBuffer;
		}
	}

	void PrimitiveIdVertexBufferPool::DiscardAll()
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

	void PrimitiveIdVertexBufferPool::Shutdown()
	{
		m_entries.clear();
	}

	int32 CachedDrawSnapshotBucket::Add( const DrawSnapshot& snapshot )
	{
		constexpr SharedSnapshotId Dummy( 0 );
		auto [iter, success] = m_bucket.emplace( snapshot, Dummy );
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

	void PreparePipelineStateObject( DrawSnapshot& snapshot )
	{
		auto& pipelineState = snapshot.m_pipelineState;
		auto& shaderState = pipelineState.m_shaderState;

		agl::GraphicsPipelineStateInitializer initializer
		{
			.m_vertexShader = shaderState.m_vertexShader ? shaderState.m_vertexShader->Resource() : nullptr,
			.m_geometryShader = shaderState.m_geometryShader ? shaderState.m_geometryShader->Resource() : nullptr,
			.m_piexlShader = shaderState.m_pixelShader ? shaderState.m_pixelShader->Resource() : nullptr,
			.m_meshShader = shaderState.m_meshShader ? shaderState.m_meshShader->Resource() : nullptr,
			.m_amplificationShader = shaderState.m_amplificationShader ? shaderState.m_amplificationShader->Resource() : nullptr,
			.m_blendState = pipelineState.m_blendState.Resource(),
			.m_rasterizerState = pipelineState.m_rasterizerState.Resource(),
			.m_depthStencilState = pipelineState.m_depthStencilState.Resource(),
			.m_vertexLayout = shaderState.m_vertexLayout.Resource(),
			.m_primitiveType = pipelineState.m_primitive,
		};

		pipelineState.m_pso = agl::GraphicsPipelineState::Create( initializer );
	}

	void SortDrawSnapshots( RenderFrameArray<VisibleDrawSnapshot>& outSnapshots )
	{
		std::ranges::sort( outSnapshots,
		                   []( const VisibleDrawSnapshot& lhs, const VisibleDrawSnapshot& rhs )
		                   {
			                   return lhs.m_snapshotBucketId < rhs.m_snapshotBucketId;
		                   } );

		MergeDrawSnapshots( outSnapshots );

		for ( size_t i = 0; i < outSnapshots.size(); ++i )
		{
			outSnapshots[i].m_primitiveIdOffset = static_cast<uint32>( i );
		}
	}

	void MergeDrawSnapshots( RenderFrameArray<VisibleDrawSnapshot>& outSnapshots )
	{
		for ( size_t cur = 0, dest = cur + 1; cur < outSnapshots.size() && dest < outSnapshots.size(); ++dest )
		{
			if ( outSnapshots[cur].m_snapshotBucketId != -1 &&
				outSnapshots[cur].m_snapshotBucketId == outSnapshots[dest].m_snapshotBucketId )
			{
				++outSnapshots[cur].m_numInstance;
			}
			else
			{
				cur = dest;
			}
		}
	}

	void UpdatePrimitiveIDs( const RenderFrameArray<VisibleDrawSnapshot>& visibleSnapshots, VertexBuffer& primitiveIds )
	{
		auto idBuffer = static_cast<uint32*>( primitiveIds.Lock() );
		if ( idBuffer )
		{
			for ( size_t i = 0; i < visibleSnapshots.size(); ++i )
			{
				*idBuffer = visibleSnapshots[i].m_primitiveId;
				++idBuffer;
			}

			primitiveIds.Unlock();
		}
	}

	void CommitDrawSnapshots( CommandList& commandList, RenderFrameArray<VisibleDrawSnapshot>& visibleSnapshots, const VertexBuffer& primitiveIds )
	{
		for ( size_t i = 0; i < visibleSnapshots.size(); )
		{
			CommitDrawSnapshot( commandList, visibleSnapshots[i], primitiveIds );
			i += visibleSnapshots[i].m_numInstance;
		}
	}

	void ParallelCommitDrawSnapshot( CommandList& commandList, RenderFrameArray<VisibleDrawSnapshot>& visibleSnapshots, const VertexBuffer& primitiveIds )
	{
		size_t dc = 0;
		for ( size_t i = 0; i < visibleSnapshots.size(); )
		{
			++dc;
			i += visibleSnapshots[i].m_numInstance;
		}

		if ( dc < 64 )
		{
			CommitDrawSnapshots( commandList, visibleSnapshots, primitiveIds );
		}
		else
		{
			auto taskScheduler = GetInterface<ITaskScheduler>();
			constexpr size_t AffinityMask = WorkerAffinityMask<WorkerThreads>();
			TaskHandle taskGroup = taskScheduler->GetTaskGroup();

			CommitDrawSnapshotTask* commitTasks[2] = {};
			agl::ICommandList* deferredCommandLists[2] = {};

			for ( size_t i = 0, j = 0; i < std::extent_v<decltype( commitTasks )>; ++i )
			{
				deferredCommandLists[i] = GraphicsInterface().GetParallelCommandList();

				size_t count = ( dc + 1 ) / 2;
				auto task = Task<CommitDrawSnapshotTask>::Create( AffinityMask, count, *deferredCommandLists[i], primitiveIds );
				commitTasks[i] = &task->Element();

				size_t added = 0;
				for ( ; j < visibleSnapshots.size() && ( added < count ); )
				{
					commitTasks[i]->AddSnapshot( &visibleSnapshots[j] );
					j += visibleSnapshots[j].m_numInstance;
					++added;
				}

				taskGroup.AddTask( task );
			}

			/*
			// TODO
			for ( size_t i = 0; i < std::extent_v<decltype( deferredCommandLists )>; ++i )
			{
				renderer.ApplyOutputContext( *deferredCommandLists[i] );
			}
			*/

			taskScheduler->Run( taskGroup );
			taskScheduler->Wait( taskGroup );
		}
	}
}
