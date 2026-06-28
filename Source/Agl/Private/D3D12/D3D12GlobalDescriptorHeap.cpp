#include "D3D12GlobalDescriptorHeap.h"

namespace agl
{
	ID3D12DescriptorHeap* D3D12GlobalHeapAllocatedInfo::GetDescriptorHeap()
	{
		return m_base.Resource();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE D3D12GlobalHeapAllocatedInfo::GetCpuHandle( uint32 offset ) const
	{
		assert( m_offset + offset < m_end );
		return m_base.GetCpuHandle().At( m_offset + offset );
	}

	D3D12_GPU_DESCRIPTOR_HANDLE D3D12GlobalHeapAllocatedInfo::GetGpuHandle( uint32 offset ) const
	{
		assert( m_offset + offset < m_end );
		return m_base.GetGpuHandle().At( m_offset + offset );
	}

	D3D12GlobalHeapAllocatedInfo::D3D12GlobalHeapAllocatedInfo( const D3D12DescriptorHeap& base, uint32 offset, uint32 end ) noexcept
		: m_base( base )
		, m_offset( offset )
		, m_end( end )
	{
	}

	void D3D12GlobalDescriptorHeap::Initialize()
	{
		MaxDescriptorHeapSize[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV] = MaxBindlessResources + MaxFrameResources * DefaultAgl::GetBufferCount();
		MaxDescriptorHeapSize[D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER] = MaxBindlessSamplers + MaxFrameSamplers * DefaultAgl::GetBufferCount();

		auto& allocator = D3D12DescriptorHeapAllocator::GetInstance();

		for ( int32 i = 0; i < NumDescriptorHeapType; ++i )
		{
			auto heapType = static_cast<D3D12_DESCRIPTOR_HEAP_TYPE>( i );
			m_heaps[i] = allocator.AllocGpuDescriptorHeap( heapType, MaxDescriptorHeapSize[i] );
			m_ringAllocator[i].Initialize( MaxBindlessDescriptorHeapSizes[i], MaxDescriptorHeapSize[i] );
		}
	}

	void D3D12GlobalDescriptorHeap::Prepare()
	{
		for ( int32 i = 0; i < NumDescriptorHeapType; ++i )
		{
			m_ringAllocator[i].Prepare();
		}
	}

	void D3D12GlobalDescriptorHeap::OnEndFrameRendering()
	{
		for ( int32 i = 0; i < NumDescriptorHeapType; ++i )
		{
			m_ringAllocator[i].OnEndFrameRendering();
		}
	}

	D3D12GlobalHeapAllocatedInfo D3D12GlobalDescriptorHeap::GetHeapStart( D3D12_DESCRIPTOR_HEAP_TYPE type ) const
	{
		assert( static_cast<uint32>( type ) < NumDescriptorHeapType );
		D3D12GlobalHeapAllocatedInfo allocatedInfo( m_heaps[type], 0, MaxDescriptorHeapSize[type] );
		return allocatedInfo;
	}

	void D3D12GlobalDescriptorHeap::UpdateBindless( D3D12_DESCRIPTOR_HEAP_TYPE type, int32 bindlessHandle, const D3D12CpuDescriptorHandle& handle )
	{
		assert( bindlessHandle != NullBindlessHandle );
		assert( static_cast<uint32>( type ) < NumDescriptorHeapType );
		assert( static_cast<uint32>( bindlessHandle ) < MaxBindlessDescriptorHeapSizes[type] );
		D3D12Device().CopyDescriptorsSimple( 1, m_heaps[type].GetCpuHandle().At( bindlessHandle ), handle.At(), type );
	}

	D3D12GlobalHeapAllocatedInfo D3D12GlobalDescriptorHeap::Acquire( D3D12_DESCRIPTOR_HEAP_TYPE type, uint32 num )
	{
		assert( static_cast<uint32>( type ) < NumDescriptorHeapType );
		uint32 offset = m_ringAllocator[type].Acquire( num );

		D3D12GlobalHeapAllocatedInfo allocatedInfo( m_heaps[type], offset, offset + num );
		return allocatedInfo;
	}

	void D3D12GlobalDescriptorHeap::RingAllocator::Initialize( uint32 begin, uint32 end )
	{
		m_begin = begin;
		m_end = end;
		m_capacity = end - begin;

		[[maybe_unused]] HRESULT hr = D3D12Device().CreateFence( m_fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS( m_fence.GetAddressOf() ) );
		assert( SUCCEEDED( hr ) );

		m_fenceEvent = CreateEvent( nullptr, false, false, _T( "Ring Allocator Fence Event" ) );
	}

	uint32 D3D12GlobalDescriptorHeap::RingAllocator::Acquire( uint32 num )
	{
		assert( num <= m_capacity && "Requested descriptor count exceeds ring allocator capacity." );
		uint32 offset = m_head % m_capacity;

		if ( offset + num > m_capacity )
		{
			m_head += m_capacity - offset;
			offset = 0;
		}

		m_head += num;
		while ( m_head - m_tail > m_capacity )
		{
			auto& frameMarker = m_frameMarkers.front();
			if ( frameMarker.m_fenceValue <= m_fence->GetCompletedValue() )
			{
				m_tail = frameMarker.m_head;
				m_frameMarkers.pop_front();
			}
			else
			{
				[[maybe_unused]] HRESULT hr = m_fence->SetEventOnCompletion( frameMarker.m_fenceValue, m_fenceEvent );
				assert( SUCCEEDED( hr ) );
				WaitForSingleObject( m_fenceEvent, INFINITE );
			}
		}

		return m_begin + offset;
	}

	void D3D12GlobalDescriptorHeap::RingAllocator::Prepare()
	{
		uint64 completedFenceValue = m_fence->GetCompletedValue();
		while ( m_frameMarkers.empty() == false )
		{
			auto& frameMarker = m_frameMarkers.front();
			if ( frameMarker.m_fenceValue <= completedFenceValue )
			{
				m_tail = frameMarker.m_head;
				m_frameMarkers.pop_front();
			}
			else
			{
				break;
			}
		}
	}

	void D3D12GlobalDescriptorHeap::RingAllocator::OnEndFrameRendering()
	{
		++m_fenceValue;
		D3D12DirectCommandQueue().Signal( m_fence.Get(), m_fenceValue );
		m_frameMarkers.emplace_back( m_head, m_fenceValue );
	}

	D3D12GlobalDescriptorHeap::RingAllocator::~RingAllocator()
	{
		CloseHandle( m_fenceEvent );
	}
}
