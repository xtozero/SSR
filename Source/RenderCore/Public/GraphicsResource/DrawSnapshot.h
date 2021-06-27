#pragma once

#include "GraphicsApiResource.h"
#include "GraphicsPipelineState.h"
#include "IndexBuffer.h"
#include "PipelineState.h"
#include "ShaderBindings.h"
#include "VertexInputStream.h"

#include <map>

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
	UINT m_primitiveId;

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
			m_primitiveId = other.m_primitiveId;
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
			m_primitiveId = other.m_primitiveId;
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

void PreparePipelineStateObject( std::vector<DrawSnapshot>& snapshots );
void SortDrawSnapshots( std::vector<DrawSnapshot>& snapshots, VertexBuffer& primitiveIds );
void CommitDrawSnapshots( std::vector<DrawSnapshot>& snapshots );
void CommitDrawSnapshot( DrawSnapshot& snapshot );