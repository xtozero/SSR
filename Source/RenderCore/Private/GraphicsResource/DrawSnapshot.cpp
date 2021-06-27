#include "stdafx.h"
#include "DrawSnapshot.h"

#include "AbstractGraphicsInterface.h"

#include <optional>

PrimitiveIdVertexBufferPool& PrimitiveIdVertexBufferPool::GetInstance( )
{
	static PrimitiveIdVertexBufferPool primitiveIdVertexBufferPool;
	return primitiveIdVertexBufferPool;
}

VertexBuffer PrimitiveIdVertexBufferPool::Alloc( std::size_t require )
{
	constexpr std::size_t minimum = 1024;
	require = ( require + minimum - 1 ) & ~( minimum - 1 );

	std::optional<std::size_t> bestMatchIdx;
	for ( std::size_t i = 0; i < m_entries.size( ); ++i )
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

		std::size_t elementSize = sizeof( UINT );
		std::size_t numElement = require / sizeof( UINT );
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

void PreparePipelineStateObject( std::vector<DrawSnapshot>& snapshots )
{
	for ( auto& snapshot : snapshots )
	{
		auto& pipelineState = snapshot.m_pipelineState;
		auto& shaderState = pipelineState.m_shaderState;

		aga::PipelineStateInitializer initializer
		{
			shaderState.m_vertexShader.Resource( ),
			shaderState.m_pixelShader.Resource( ),
			pipelineState.m_blendState.Resource( ),
			pipelineState.m_rasterizerState.Resource( ),
			pipelineState.m_depthStencilState.Resource( ),
			shaderState.m_vertexLayout.Resource( ),
			pipelineState.m_primitive,
		};

		pipelineState.m_pso = aga::PipelineState::Create( initializer );
	}
}

void SortDrawSnapshots( std::vector<DrawSnapshot>& snapshots, VertexBuffer& primitiveIds )
{
	UINT* idBuffer = reinterpret_cast<UINT*>( primitiveIds.Lock( ) );
	if ( idBuffer )
	{
		for ( std::size_t i = 0; i < snapshots.size(); ++i )
		{
			snapshots[i].m_vertexStream.Bind( primitiveIds, 1, static_cast<UINT>( i ) * sizeof( UINT ) );
			*idBuffer = snapshots[i].m_primitiveId;
			++idBuffer;
		}

		primitiveIds.Unlock( );
	}
}

void CommitDrawSnapshots( std::vector<DrawSnapshot>& snapshots )
{
	for ( auto& snapshot : snapshots )
	{
		CommitDrawSnapshot( snapshot );
	}
}

void CommitDrawSnapshot( DrawSnapshot& snapshot )
{
	auto commandList = GetInterface<aga::IAga>( )->GetImmediateCommandList( );

	// Set vertex buffer
	int numVB = snapshot.m_vertexStream.NumBuffer( );
	aga::Buffer* const* vertexBuffers = snapshot.m_vertexStream.VertexBuffers( );
	const UINT* vertexOffsets = snapshot.m_vertexStream.Offsets( );
	commandList->BindVertexBuffer( vertexBuffers, 0, numVB, vertexOffsets );

	// Set index buffer
	aga::Buffer* indexBuffer = snapshot.m_indexBuffer.Resource( );
	UINT indexOffset = 0;

	commandList->BindIndexBuffer( indexBuffer, indexOffset );

	// Set pipeline state
	commandList->BindPipelineState( snapshot.m_pipelineState.m_pso );

	// Set shader resources
	commandList->BindShaderResources( snapshot.m_shaderBindings );

	commandList->Draw( snapshot.m_indexCount, snapshot.m_startIndexLocation, snapshot.m_baseVertexLocation );
}
