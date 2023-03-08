#include "D3D12GlobalDescriptorHeap.h"

namespace agl
{
	D3D12GlobalHeapAllocatedInfo D3D12GlobalDescriptorHeap::Aquire( D3D12_DESCRIPTOR_HEAP_TYPE type, uint32 num )
	{
		DescriptorHeapInfo& lastest = GetLastestHeap( type );
		if ( lastest.m_capacity - lastest.m_size < num )
		{
			++m_top[type];
			if ( m_top[type] >= m_heaps[type].size() )
			{
				m_heaps[type].push_back( AllocateDefaultHeap( type ) );
			}

			lastest = GetLastestHeap( type );
		}

		D3D12GlobalHeapAllocatedInfo allocatedInfo = {
			.m_base = lastest.m_heap,
			.m_offset = lastest.m_size,
			.m_end = lastest.m_size + num
		};

		lastest.m_size += num;

		return allocatedInfo;
	}

	D3D12GlobalDescriptorHeap::DescriptorHeapInfo D3D12GlobalDescriptorHeap::AllocateDefaultHeap( D3D12_DESCRIPTOR_HEAP_TYPE type )
	{
		DescriptorHeapInfo newHeap = {
				.m_heap = D3D12DescriptorHeapAllocator::GetInstance().AllocGpuDescriptorHeap( type, DefaultHeapSize ),
				.m_capacity = DefaultHeapSize,
				.m_size = 0
		};

		return newHeap;
	}

	D3D12GlobalDescriptorHeap::DescriptorHeapInfo& D3D12GlobalDescriptorHeap::GetLastestHeap( D3D12_DESCRIPTOR_HEAP_TYPE type )
	{
		if ( m_heaps[type].empty() )
		{
			m_heaps[type].push_back( AllocateDefaultHeap( type ) );
		}

		return m_heaps[type][m_top[type]];
	}
}