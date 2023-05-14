#include "D3D12GlobalDescriptorHeap.h"

namespace agl
{
	void D3D12GlobalDescriptorHeap::Prepare()
	{
		for ( int32 i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i )
		{
			m_top[i] = 0;
			for ( DescriptorHeapInfo& descriptorHeap : m_heaps[i] )
			{
				descriptorHeap.m_size = 0;
			}
		}
	}

	D3D12GlobalHeapAllocatedInfo D3D12GlobalDescriptorHeap::Aquire( D3D12_DESCRIPTOR_HEAP_TYPE type, uint32 num )
	{
		DescriptorHeapInfo* lastest = GetLastestHeap( type );
		if ( lastest->m_capacity - lastest->m_size < num )
		{
			++m_top[type];
			if ( m_top[type] >= m_heaps[type].size() )
			{
				m_heaps[type].push_back( AllocateDefaultHeap( type ) );
			}

			lastest = GetLastestHeap( type );
		}

		D3D12GlobalHeapAllocatedInfo allocatedInfo( lastest->m_heap, lastest->m_size, lastest->m_size + num );
		lastest->m_size += num;

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

	D3D12GlobalDescriptorHeap::DescriptorHeapInfo* D3D12GlobalDescriptorHeap::GetLastestHeap( D3D12_DESCRIPTOR_HEAP_TYPE type )
	{
		if ( m_heaps[type].empty() )
		{
			m_heaps[type].push_back( AllocateDefaultHeap( type ) );
		}

		return &m_heaps[type][m_top[type]];
	}

	ID3D12DescriptorHeap* D3D12GlobalHeapAllocatedInfo::GetDescriptorHeap()
	{
		return m_base.Resource();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE D3D12GlobalHeapAllocatedInfo::GetCpuHandle( uint32 offset ) const
	{
		return m_base.GetCpuHandle().At( m_offset + offset );
	}

	D3D12_GPU_DESCRIPTOR_HANDLE D3D12GlobalHeapAllocatedInfo::GetGpuHandle( uint32 offset ) const
	{
		return m_base.GetGpuHandle().At( m_offset + offset );
	}

	D3D12GlobalHeapAllocatedInfo::D3D12GlobalHeapAllocatedInfo( const D3D12DescriptorHeap& base, uint32 offset, uint32 end ) noexcept
		: m_base( base )
		, m_offset( offset )
		, m_end( end )
	{
	}
}