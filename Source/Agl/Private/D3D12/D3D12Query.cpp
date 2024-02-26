#include "D3D12Query.h"

#include "D3D12Api.h"
#include "D3D12CommandList.h"
#include "ICommandList.h"
#include "TaskScheduler.h"

#include <cassert>
#include <numeric>

namespace
{
	D3D12_QUERY_HEAP_TYPE GetQueryHeapType( D3D12_QUERY_TYPE type )
	{
		switch ( type )
		{
		case D3D12_QUERY_TYPE_OCCLUSION:
			return D3D12_QUERY_HEAP_TYPE_OCCLUSION;
			break;
		case D3D12_QUERY_TYPE_BINARY_OCCLUSION:
			return D3D12_QUERY_HEAP_TYPE_OCCLUSION;
			break;
		case D3D12_QUERY_TYPE_TIMESTAMP:
			return D3D12_QUERY_HEAP_TYPE_TIMESTAMP;
			break;
		case D3D12_QUERY_TYPE_PIPELINE_STATISTICS:
			return D3D12_QUERY_HEAP_TYPE_PIPELINE_STATISTICS;
			break;
		case D3D12_QUERY_TYPE_SO_STATISTICS_STREAM0:
			[[fallthrough]];
		case D3D12_QUERY_TYPE_SO_STATISTICS_STREAM1:
			[[fallthrough]];
		case D3D12_QUERY_TYPE_SO_STATISTICS_STREAM2:
			[[fallthrough]];
		case D3D12_QUERY_TYPE_SO_STATISTICS_STREAM3:
			return D3D12_QUERY_HEAP_TYPE_SO_STATISTICS;
			break;
		case D3D12_QUERY_TYPE_VIDEO_DECODE_STATISTICS:
			return D3D12_QUERY_HEAP_TYPE_VIDEO_DECODE_STATISTICS;
			break;
		case D3D12_QUERY_TYPE_PIPELINE_STATISTICS1:
			return D3D12_QUERY_HEAP_TYPE_PIPELINE_STATISTICS1;
			break;
		}

		return D3D12_QUERY_HEAP_TYPE_OCCLUSION;
	}
}

namespace agl
{
	void D3D12QueryHeapBlock::InitResource( D3D12_QUERY_HEAP_TYPE type )
	{
		constexpr uint32 DefaultBlockSize = 64 * 1024 * 1024; // 64mb

		m_freeSize = DefaultBlockSize / sizeof( uint64 );
		m_freeList.resize( m_freeSize );
		std::iota( std::rbegin( m_freeList ), std::rend( m_freeList ), 0 );

		D3D12_QUERY_HEAP_DESC heapDesc = {
			.Type = type,
			.Count = m_freeSize,
			.NodeMask = 0
		};

		HRESULT hr = D3D12Device().CreateQueryHeap( &heapDesc, IID_PPV_ARGS( m_heap.GetAddressOf() ) );

		assert( SUCCEEDED( hr ) );

		D3D12_HEAP_PROPERTIES bufferHeapProperties = {
			.Type = D3D12_HEAP_TYPE_READBACK,
			.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
			.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN,
			.CreationNodeMask = 0,
			.VisibleNodeMask = 0
		};

		D3D12_RESOURCE_DESC bufferDesc = {
			.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
			.Alignment = 0,
			.Width = DefaultBlockSize,
			.Height = 1,
			.DepthOrArraySize = 1,
			.MipLevels = 1,
			.Format = DXGI_FORMAT_UNKNOWN,
			.SampleDesc = {
				.Count = 1,
				.Quality = 0,
			},
			.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
			.Flags = D3D12_RESOURCE_FLAG_NONE
		};

		hr = D3D12Device().CreateCommittedResource( &bufferHeapProperties, D3D12_HEAP_FLAG_NONE, &bufferDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS( &m_readBackBuffer ) );

		assert( SUCCEEDED( hr ) );

		m_readBackBuffer->Map( 0, nullptr, &m_readBackPtr );
	}

	D3D12Query D3D12QueryHeapBlock::Allocate( D3D12_QUERY_TYPE type )
	{
		--m_freeSize;

		return D3D12Query{
			.m_type = type,
			.m_heap = this,
			.m_offset = m_freeList[m_freeSize] };
	}

	void D3D12QueryHeapBlock::Deallocate( const D3D12Query& query )
	{
		m_freeList[m_freeSize] = query.m_offset;

		++m_freeSize;
	}

	bool D3D12QueryHeapBlock::CanAllocate() const
	{
		return m_freeSize > 0;
	}

	bool D3D12QueryHeapBlock::IsOwned( const D3D12Query& query ) const
	{
		return query.m_heap == this;
	}

	ID3D12QueryHeap* D3D12QueryHeapBlock::GetHeap()
	{
		return m_heap.Get();
	}

	ID3D12Resource* D3D12QueryHeapBlock::GetReadBackBuffer()
	{
		return m_readBackBuffer.Get();
	}

	void D3D12QueryHeapBlock::GetData( void* outData, int32 size, int32 offset )
	{
		auto src = static_cast<uint8*>( m_readBackPtr ) + size * offset;
		std::memcpy( outData, src, size );
		*(int64*)src = 0;
	}

	D3D12QueryHeapBlock::~D3D12QueryHeapBlock()
	{
		m_readBackBuffer->Unmap( 0, nullptr );
	}

	D3D12Query D3D12QueryHeapBudget::Allocate( D3D12_QUERY_TYPE type )
	{
		assert( IsInRenderThread() );

		D3D12QueryHeapBlock* block = FindProperBlock();
		if ( block == nullptr )
		{
			block = &m_blocks.emplace_back();
			block->InitResource( GetQueryHeapType( type ) );
		}

		return block->Allocate( type );
	}

	void D3D12QueryHeapBudget::Deallocate( const D3D12Query& query )
	{
		assert( IsInRenderThread() );

		for ( D3D12QueryHeapBlock& block : m_blocks )
		{
			if ( block.IsOwned( query ) )
			{
				block.Deallocate( query );
				return;
			}
		}
	}

	D3D12QueryHeapBlock* D3D12QueryHeapBudget::FindProperBlock()
	{
		for ( D3D12QueryHeapBlock& block : m_blocks )
		{
			if ( block.CanAllocate() )
			{
				return &block;
			}
		}

		return nullptr;
	}

	D3D12Query D3D12QueryAllocator::Allocate( D3D12_QUERY_TYPE type )
	{
		uint32 heapIndex = GetHeapIndex( type );

		assert( 0 <= heapIndex && heapIndex < MaxQueryHeapType );
		return m_heaps[heapIndex].Allocate( type );
	}

	void D3D12QueryAllocator::Deallocate( const D3D12Query& query )
	{
		uint32 heapIndex = GetHeapIndex( query.m_type );

		assert( 0 <= heapIndex && heapIndex < MaxQueryHeapType );
		m_heaps[heapIndex].Deallocate( query );
	}

	uint32 D3D12QueryAllocator::GetHeapIndex( D3D12_QUERY_TYPE type )
	{
		switch ( type )
		{
		case D3D12_QUERY_TYPE_OCCLUSION:
			[[fallthrough]];
		case D3D12_QUERY_TYPE_BINARY_OCCLUSION:
			return 0;
			break;
		case D3D12_QUERY_TYPE_TIMESTAMP:
			return 1;
			break;
		case D3D12_QUERY_TYPE_PIPELINE_STATISTICS:
			return 2;
			break;
		case D3D12_QUERY_TYPE_SO_STATISTICS_STREAM0:
			[[fallthrough]];
		case D3D12_QUERY_TYPE_SO_STATISTICS_STREAM1:
			[[fallthrough]];
		case D3D12_QUERY_TYPE_SO_STATISTICS_STREAM2:
			[[fallthrough]];
		case D3D12_QUERY_TYPE_SO_STATISTICS_STREAM3:
			return 3;
			break;
		case D3D12_QUERY_TYPE_VIDEO_DECODE_STATISTICS:
			return 4;
			break;
		case D3D12_QUERY_TYPE_PIPELINE_STATISTICS1:
			return 5;
			break;
		}

		assert( false && "GetHeapIndex - Invalid query type" );
		return 0;
	}

	void D3D12GpuTimer::InitResource()
	{
		m_timeStampBegin = D3D12AllocatorForQuery().Allocate( D3D12_QUERY_TYPE_TIMESTAMP );
		m_timeStampEnd = D3D12AllocatorForQuery().Allocate( D3D12_QUERY_TYPE_TIMESTAMP );
	}

	void D3D12GpuTimer::FreeResource()
	{
		D3D12AllocatorForQuery().Deallocate( m_timeStampBegin );
		D3D12AllocatorForQuery().Deallocate( m_timeStampEnd );
	}

	void D3D12GpuTimer::Begin( ICommandListBase& commandList )
	{
		commandList.EndQuery( &m_timeStampBegin );
	}

	void D3D12GpuTimer::End( ICommandListBase& commandList )
	{
		auto& d3d12CommandList = static_cast<ID3D12CommandListEX&>( commandList );
		d3d12CommandList.EndQuery( &m_timeStampEnd );

		bool isContinuous = ( m_timeStampBegin.m_heap == m_timeStampEnd.m_heap )
			&& ( std::abs( m_timeStampBegin.m_offset - m_timeStampEnd.m_offset ) == 1 );
		if ( isContinuous )
		{
			int32 offset = std::min( m_timeStampBegin.m_offset, m_timeStampEnd.m_offset );
			d3d12CommandList.ResolveQueryData( m_timeStampBegin.m_heap, D3D12_QUERY_TYPE_TIMESTAMP, offset, 2 );
		}
		else
		{
			d3d12CommandList.ResolveQueryData( m_timeStampBegin.m_heap, D3D12_QUERY_TYPE_TIMESTAMP, m_timeStampBegin.m_offset, 1 );
			d3d12CommandList.ResolveQueryData( m_timeStampEnd.m_heap, D3D12_QUERY_TYPE_TIMESTAMP, m_timeStampEnd.m_offset, 1 );
		}
	}

	double D3D12GpuTimer::GetDuration()
	{
		uint64 beginTime = 0;
		m_timeStampBegin.m_heap->GetData( &beginTime, sizeof( beginTime ), m_timeStampBegin.m_offset );

		uint64 endTime = 0;
		m_timeStampEnd.m_heap->GetData( &endTime, sizeof( endTime ), m_timeStampEnd.m_offset );

		uint64 gpuFrequency = 0;
		D3D12DirectCommandQueue().GetTimestampFrequency( &gpuFrequency );

		if ( endTime > beginTime )
		{
			auto deltaTime = static_cast<double>( endTime - beginTime );
			auto frequency = static_cast<double>( gpuFrequency );

			return deltaTime / frequency * 1000.0; // millisecond
		}

		return 0.0;
	}
};