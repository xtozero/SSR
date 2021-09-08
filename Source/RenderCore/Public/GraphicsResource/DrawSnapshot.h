#pragma once

#include "GraphicsApiResource.h"
#include "GraphicsPipelineState.h"
#include "IndexBuffer.h"
#include "PipelineState.h"
#include "ShaderBindings.h"
#include "SparseArray.h"
#include "SizedTypes.h"
#include "VertexInputStream.h"

#include <map>
#include <unordered_map>

class PrimitiveIdVertexBufferPool
{
public:
	static PrimitiveIdVertexBufferPool& GetInstance( );

	VertexBuffer Alloc( std::size_t require );
	void DiscardAll( );

protected:
	PrimitiveIdVertexBufferPool( ) = default;

private:
	struct PrimitiveIdVertexBufferPoolEntry
	{
		std::size_t m_lastDiscardId;
		VertexBuffer m_vertexBuffer;
	};

	std::vector<PrimitiveIdVertexBufferPoolEntry> m_entries;
	std::size_t m_discardId = 0;
};

class DrawSnapshot
{
public:
	VertexInputStream m_vertexStream;
	IndexBuffer m_indexBuffer;

	aga::ShaderBindings m_shaderBindings;

	// Pipeline State
	GraphicsPipelineState m_pipelineState;

	// For Draw method
	UINT m_indexCount;
	UINT m_startIndexLocation;
	UINT m_baseVertexLocation;

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
			m_indexBuffer = other.m_indexBuffer;
			m_shaderBindings = other.m_shaderBindings;
			m_pipelineState = other.m_pipelineState;
			m_indexCount = other.m_indexCount;
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
			m_indexBuffer = std::move( other.m_indexBuffer );
			m_shaderBindings = std::move( other.m_shaderBindings );
			m_pipelineState = std::move( other.m_pipelineState );
			m_indexCount = other.m_indexCount;
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
			lhs.m_indexBuffer == rhs.m_indexBuffer &&
			lhs.m_shaderBindings.MatchsForDynamicInstancing( rhs.m_shaderBindings ) &&
			lhs.m_pipelineState.m_pso == rhs.m_pipelineState.m_pso &&
			lhs.m_indexCount == rhs.m_indexCount &&
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
		for ( int32 i = 0; i < ds.m_vertexStream.NumBuffer( ); ++i )
		{
			HashCombine( hash, i );
			HashCombine( hash, offsets[i] );
			HashCombine( hash, vertexBuffers[i] );
		}

		HashCombine( hash, ds.m_indexBuffer.Resource( ) );

		HashCombine( hash, ds.m_shaderBindings.HashForDynamicInstaning( ) );

		HashCombine( hash, ds.m_pipelineState.m_pso.Get( ) );
		HashCombine( hash, ds.m_indexCount );
		HashCombine( hash, ds.m_startIndexLocation );
		HashCombine( hash, ds.m_baseVertexLocation );

		return hash;
	}
};

class VisibleDrawSnapshot
{
public:
	UINT m_primitiveId;
	UINT m_primitiveIdOffset;
	UINT m_numInstance;
	int32 m_snapshotBucketId;
	DrawSnapshot* m_drawSnapshot;
};

struct CachedDrawSnapshotInfo
{
	size_t m_snapshotIndex;
	int32 m_snapshotBucketId;
};

class CachedDrawSnapshotBucket
{
public:
	int32 Add( const DrawSnapshot& snapshot );
	void Remove( int32 id );

private:
	std::unordered_map<DrawSnapshot, size_t, DrawSnapshotDynamicInstancingHasher, DrawSnapshotDynamicInstancingEqual> m_bucket;
	SparseArray<DrawSnapshot> m_snapshots;
};

void PreparePipelineStateObject( DrawSnapshot& snapshot );
void SortDrawSnapshots( std::vector<VisibleDrawSnapshot>& visibleSnapshots, VertexBuffer& primitiveIds );
void CommitDrawSnapshots( std::vector<VisibleDrawSnapshot>& visibleSnapshots, VertexBuffer& primitiveIds );
void CommitDrawSnapshot( VisibleDrawSnapshot& visibleSnapshot, VertexBuffer& primitiveIds );