#pragma once

#include "D3D12DescriptorHeapAllocator.h"
#include "SizedTypes.h"

#include <d3d12.h>
#include <vector>

namespace agl
{
	struct D3D12GlobalHeapAllocatedInfo final
	{
		D3D12DescriptorHeap m_base;
		uint32 m_offset = 0;
		uint32 m_end = 0;
	};

	class D3D12GlobalDescriptorHeap final
	{
	public:
		D3D12GlobalHeapAllocatedInfo Aquire( D3D12_DESCRIPTOR_HEAP_TYPE type, uint32 num );

	private:
		constexpr static uint32 DefaultHeapSize = 1024;

		struct DescriptorHeapInfo final
		{
			D3D12DescriptorHeap m_heap;
			uint32 m_capacity = 0;
			uint32 m_size = 0;
		};

		DescriptorHeapInfo AllocateDefaultHeap( D3D12_DESCRIPTOR_HEAP_TYPE type );
		DescriptorHeapInfo& GetLastestHeap( D3D12_DESCRIPTOR_HEAP_TYPE type );

		std::vector<DescriptorHeapInfo> m_heaps[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
		uint32 m_top[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES] = {};
	};
}