#pragma once

#include "GraphicsApiResource.h"
#include "GraphicsPipelineState.h"
#include "ICommandList.h"
#include "IndexBuffer.h"
#include "PipelineState.h"
#include "ShaderBindings.h"
#include "SparseArray.h"
#include "SizedTypes.h"
#include "VertexBufferBundle.h"

#include <map>
#include <unordered_map>

class RenderViewGroup;
class SceneRenderer;

enum class RenderPass;

class PrimitiveIdVertexBufferPool
{
public:
	static PrimitiveIdVertexBufferPool& GetInstance( );

	VertexBuffer Alloc( uint32 require );
	void DiscardAll( );

protected:
	PrimitiveIdVertexBufferPool( ) = default;

private:
	struct PrimitiveIdVertexBufferPoolEntry
	{
		size_t m_lastDiscardId;
		VertexBuffer m_vertexBuffer;
	};

	std::vector<PrimitiveIdVertexBufferPoolEntry> m_entries;
	size_t m_discardId = 0;
};

class DrawSnapshot
{
public:
	VertexBufferBundle m_vertexStream;
	int32 m_primitiveIdSlot;

	IndexBuffer m_indexBuffer;

	aga::ShaderBindings m_shaderBindings;

	// Pipeline State
	GraphicsPipelineState m_pipelineState;

	// For Draw method
	uint32 m_count;
	uint32 m_startIndexLocation;
	uint32 m_baseVertexLocation;

	DrawSnapshot( ) = default;
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

struct DrawSnapshotDynamicInstancingEqual
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

struct DrawSnapshotDynamicInstancingHasher
{
	size_t operator()( const DrawSnapshot& ds ) const
	{
		static size_t typeHash = typeid( DrawSnapshotDynamicInstancingHasher ).hash_code( );
		size_t hash = typeHash;

		const aga::Buffer* const* vertexBuffers = ds.m_vertexStream.VertexBuffers( );
		const uint32* const offsets = ds.m_vertexStream.Offsets( );
		for ( uint32 i = 0; i < ds.m_vertexStream.NumBuffer( ); ++i )
		{
			HashCombine( hash, i );
			HashCombine( hash, offsets[i] );
			HashCombine( hash, vertexBuffers[i] );
		}

		HashCombine( hash, ds.m_primitiveIdSlot );

		HashCombine( hash, ds.m_indexBuffer.Resource( ) );

		HashCombine( hash, ds.m_shaderBindings.HashForDynamicInstaning( ) );

		HashCombine( hash, ds.m_pipelineState.m_pso.Get( ) );
		HashCombine( hash, ds.m_count );
		HashCombine( hash, ds.m_startIndexLocation );
		HashCombine( hash, ds.m_baseVertexLocation );

		return hash;
	}
};

class VisibleDrawSnapshot
{
public:
	uint32 m_primitiveId;
	uint32 m_primitiveIdOffset;
	uint32 m_numInstance;
	int32 m_snapshotBucketId;
	DrawSnapshot* m_drawSnapshot;
};

struct CachedDrawSnapshotInfo
{
	RenderPass m_renderPass;
	size_t m_snapshotIndex;
	int32 m_snapshotBucketId;
};

class CachedDrawSnapshotBucket
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

void PreparePipelineStateObject( DrawSnapshot& snapshot );
void SortDrawSnapshots( std::vector<VisibleDrawSnapshot>& visibleSnapshots, VertexBuffer& primitiveIds );
void CommitDrawSnapshots( SceneRenderer& renderer, std::vector<VisibleDrawSnapshot>& visibleSnapshots, VertexBuffer& primitiveIds );
void CommitDrawSnapshot( aga::ICommandList& commandList, VisibleDrawSnapshot& visibleSnapshot, VertexBuffer& primitiveIds );
void ParallelCommitDrawSnapshot( SceneRenderer& renderer, std::vector<VisibleDrawSnapshot>& visibleSnapshots, VertexBuffer& primitiveIds );