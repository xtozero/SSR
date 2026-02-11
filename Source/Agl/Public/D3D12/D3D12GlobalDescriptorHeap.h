#pragma once

#include "D3D12DescriptorHeapAllocator.h"
#include "SizedTypes.h"

#include <d3d12.h>
#include <vector>

namespace agl
{
	class D3D12GlobalHeapAllocatedInfo final
	{
	public:
		ID3D12DescriptorHeap* GetDescriptorHeap();
		D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle( uint32 offset = 0 ) const;
		D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle( uint32 offset = 0 ) const;

		D3D12GlobalHeapAllocatedInfo( const D3D12DescriptorHeap& base, uint32 offset, uint32 end ) noexcept;
		D3D12GlobalHeapAllocatedInfo( const D3D12GlobalHeapAllocatedInfo& ) = default;
		D3D12GlobalHeapAllocatedInfo& operator=( const D3D12GlobalHeapAllocatedInfo& ) = default;
		D3D12GlobalHeapAllocatedInfo( D3D12GlobalHeapAllocatedInfo&& ) = default;
		D3D12GlobalHeapAllocatedInfo& operator=( D3D12GlobalHeapAllocatedInfo&& ) = default;

	private:
		D3D12DescriptorHeap m_base;
		uint32 m_offset = 0;
		uint32 m_end = 0;
	};

	class D3D12GlobalDescriptorHeap final
	{
	public:
		void Prepare();
		D3D12GlobalHeapAllocatedInfo Aquire( D3D12_DESCRIPTOR_HEAP_TYPE type, uint32 num );

		D3D12GlobalDescriptorHeap();

	private:
		static constexpr uint32 DefaultHeapCapacity = 1024;

		struct DescriptorHeapInfo final
		{
			D3D12DescriptorHeap m_heap;
			uint32 m_capacity = 0;
			uint32 m_size = 0;
		};

		uint32 CalcHeapCapacity( uint32 oldCapacity, uint32 newSize ) const;
		uint32 GetHeapCapacity( D3D12_DESCRIPTOR_HEAP_TYPE type ) const;

		DescriptorHeapInfo AllocateDefaultHeap( D3D12_DESCRIPTOR_HEAP_TYPE type );
		DescriptorHeapInfo* GetLastestHeap( D3D12_DESCRIPTOR_HEAP_TYPE type );

		uint32 m_curHeapCapacity[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];

		std::vector<DescriptorHeapInfo> m_heaps[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
		uint32 m_top[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES] = {};
	};
}