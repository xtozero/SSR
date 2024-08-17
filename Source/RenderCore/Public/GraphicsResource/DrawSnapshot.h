#pragma once

#include "GraphicsApiResource.h"
#include "GraphicsPipelineState.h"
#include "IndexBuffer.h"
#include "PipelineState.h"
#include "ShaderBindings.h"
#include "SizedTypes.h"
#include "SparseArray.h"
#include "TransientAllocator.h"
#include "VertexBufferBundle.h"

#include <map>
#include <unordered_map>

namespace rendercore
{
	class RenderViewGroup;
	class SceneRenderer;

	enum class RenderPass : uint8;

	class PrimitiveIdVertexBufferPool final
	{
	public:
		VertexBuffer Alloc( uint32 require );
		void DiscardAll();

		void Shutdown();

	private:
		struct PrimitiveIdVertexBufferPoolEntry
		{
			size_t m_lastDiscardId = 0;
			VertexBuffer m_vertexBuffer;
		};

		std::vector<PrimitiveIdVertexBufferPoolEntry> m_entries;
		size_t m_discardId = 0;
	};

	class DrawSnapshot final
	{
	public:
		VertexBufferBundle m_vertexStream;
		int32 m_primitiveIdSlot = 0;

		IndexBuffer m_indexBuffer;

		agl::ShaderBindings m_shaderBindings;

		// Pipeline State
		GraphicsPipelineState m_pipelineState;

		// For Draw method
		uint32 m_count = 0;
		uint32 m_startIndexLocation = 0;
		uint32 m_baseVertexLocation = 0;

		DrawSnapshot() = default;
		DrawSnapshot( const DrawSnapshot& other )
		{
			( *this ) = other;
		}

		DrawSnapshot& operator=( const DrawSnapshot& other )
		{
			if ( this != &other )
			{
				m_vertexStream = other.m_vertexStream;
				m_primitiveIdSlot = other.m_primitiveIdSlot;
				m_indexBuffer = other.m_indexBuffer;
				m_shaderBindings = other.m_shaderBindings;
				m_pipelineState = other.m_pipelineState;
				m_count = other.m_count;
				m_startIndexLocation = other.m_startIndexLocation;
				m_baseVertexLocation = other.m_baseVertexLocation;
			}

			return *this;
		}

		DrawSnapshot( DrawSnapshot&& other ) noexcept
		{
			( *this ) = std::move( other );
		}

		DrawSnapshot& operator=( DrawSnapshot&& other ) noexcept
		{
			if ( this != &other )
			{
				m_vertexStream = std::move( other.m_vertexStream );
				m_primitiveIdSlot = other.m_primitiveIdSlot;
				m_indexBuffer = std::move( other.m_indexBuffer );
				m_shaderBindings = std::move( other.m_shaderBindings );
				m_pipelineState = std::move( other.m_pipelineState );
				m_count = other.m_count;
				m_startIndexLocation = other.m_startIndexLocation;
				m_baseVertexLocation = other.m_baseVertexLocation;
			}

			return *this;
		}
	};

	struct DrawSnapshotDynamicInstancingEqual final
	{
		bool operator()( const DrawSnapshot& lhs, const DrawSnapshot& rhs ) const
		{
			return lhs.m_vertexStream == rhs.m_vertexStream &&
				lhs.m_primitiveIdSlot == rhs.m_primitiveIdSlot &&
				lhs.m_indexBuffer == rhs.m_indexBuffer &&
				lhs.m_shaderBindings.MatchsForDynamicInstancing( rhs.m_shaderBindings ) &&
				lhs.m_pipelineState.m_pso == rhs.m_pipelineState.m_pso &&
				lhs.m_count == rhs.m_count &&
				lhs.m_startIndexLocation == rhs.m_startIndexLocation &&
				lhs.m_baseVertexLocation == rhs.m_baseVertexLocation;
		}
	};

	struct DrawSnapshotDynamicInstancingHasher final
	{
		size_t operator()( const DrawSnapshot& ds ) const
		{
			static size_t typeHash = typeid( DrawSnapshotDynamicInstancingHasher ).hash_code();
			size_t hash = typeHash;

			const agl::Buffer* const* vertexBuffers = ds.m_vertexStream.VertexBuffers();
			const uint32* const offsets = ds.m_vertexStream.Offsets();
			for ( uint32 i = 0; i < ds.m_vertexStream.NumBuffer(); ++i )
			{
				HashCombine( hash, i );
				HashCombine( hash, offsets[i] );
				HashCombine( hash, vertexBuffers[i] );
			}

			HashCombine( hash, ds.m_primitiveIdSlot );

			HashCombine( hash, ds.m_indexBuffer.Resource() );

			HashCombine( hash, ds.m_shaderBindings.HashForDynamicInstaning() );

			HashCombine( hash, ds.m_pipelineState.m_pso.Get() );
			HashCombine( hash, ds.m_count );
			HashCombine( hash, ds.m_startIndexLocation );
			HashCombine( hash, ds.m_baseVertexLocation );

			return hash;
		}
	};

	struct VisibleDrawSnapshot final
	{
		uint32 m_primitiveId;
		uint32 m_primitiveIdOffset;
		uint32 m_numInstance;
		int32 m_snapshotBucketId;
		DrawSnapshot* m_drawSnapshot;
	};

	struct CachedDrawSnapshotInfo final
	{
		RenderPass m_renderPass;
		size_t m_snapshotIndex;
		int32 m_snapshotBucketId;
	};

	class CachedDrawSnapshotBucket final
	{
	public:
		int32 Add( const DrawSnapshot& snapshot );
		void Remove( int32 id );

	private:
		struct SharedSnapshotId
		{
			constexpr explicit SharedSnapshotId( int32 id ) : m_id( id ) {}

			int32 m_id = -1;
			int32 m_ref = 0;
		};

		std::unordered_map<DrawSnapshot, SharedSnapshotId, DrawSnapshotDynamicInstancingHasher, DrawSnapshotDynamicInstancingEqual> m_bucket;
		SparseArray<DrawSnapshot> m_snapshots;
	};

	template <typename CommandList>
	void CommitDrawSnapshot( CommandList& commandList, VisibleDrawSnapshot& visibleSnapshot, VertexBuffer& primitiveIds )
	{
		DrawSnapshot& snapshot = *visibleSnapshot.m_drawSnapshot;

		// Set vertex buffer
		VertexBufferBundle vertexStream = snapshot.m_vertexStream;
		if ( snapshot.m_primitiveIdSlot != -1 )
		{
			vertexStream.Bind( primitiveIds, static_cast<uint32>( snapshot.m_primitiveIdSlot ), primitiveIds.ElementSize(), visibleSnapshot.m_primitiveIdOffset * sizeof(uint32));
		}

		uint32 numVB = vertexStream.NumBuffer();
		agl::Buffer* const* vertexBuffers = vertexStream.VertexBuffers();
		const uint32* vertexStrides = vertexStream.Strides();
		const uint32* vertexOffsets = vertexStream.Offsets();
		commandList.BindVertexBuffer( vertexBuffers, 0, numVB, vertexStrides, vertexOffsets );

		// Set index buffer
		agl::Buffer* indexBuffer = snapshot.m_indexBuffer.Resource();
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

	void PreparePipelineStateObject( DrawSnapshot& snapshot );
	void SortDrawSnapshots( RenderThreadFrameData<VisibleDrawSnapshot>& visibleSnapshots, VertexBuffer& primitiveIds );
	void CommitDrawSnapshots( SceneRenderer& renderer, RenderThreadFrameData<VisibleDrawSnapshot>& visibleSnapshots, VertexBuffer& primitiveIds );
	void ParallelCommitDrawSnapshot( SceneRenderer& renderer, RenderThreadFrameData<VisibleDrawSnapshot>& visibleSnapshots, VertexBuffer& primitiveIds );
}
