#pragma once

#include "SizedTypes.h"

#include <d3d12.h>
#include <wrl/client.h>

namespace agl
{
	class D3D12DescriptorHandle
	{
	protected:
		explicit D3D12DescriptorHandle( uint32 increametSize )
			: m_increametSize( increametSize )
		{}
		D3D12DescriptorHandle() = default;

		uint32 m_increametSize = 0;
	};

	class D3D12CpuDescriptorHandle : public D3D12DescriptorHandle
	{
		friend class D3D12DescriptorHeap;

	public:
		D3D12_CPU_DESCRIPTOR_HANDLE At( int32 offset = 0 ) const;
		D3D12CpuDescriptorHandle Offset( int32 offset ) const;

		D3D12CpuDescriptorHandle( D3D12_CPU_DESCRIPTOR_HANDLE handle, uint32 increametSize );

	private:
		D3D12CpuDescriptorHandle() = default;
		D3D12_CPU_DESCRIPTOR_HANDLE m_handle = {};
	};

	class D3D12GpuDescriptorHandle : public D3D12DescriptorHandle
	{
		friend class D3D12DescriptorHeap;

	public:
		D3D12_GPU_DESCRIPTOR_HANDLE At( int32 offset = 0 ) const;
		D3D12GpuDescriptorHandle Offset( int32 offset ) const;

		D3D12GpuDescriptorHandle( D3D12_GPU_DESCRIPTOR_HANDLE handle, uint32 increametSize );

	private:
		D3D12GpuDescriptorHandle() = default;
		D3D12_GPU_DESCRIPTOR_HANDLE m_handle = {};
	};

	class D3D12DescriptorHeap
	{
	public:
		ID3D12DescriptorHeap* Resource()
		{
			return m_heap.Get();
		}

		const ID3D12DescriptorHeap* Resource() const
		{
			return m_heap.Get();
		}

		const D3D12CpuDescriptorHandle& GetCpuHandle() const
		{
			return m_cpuHandle;
		}

		const D3D12GpuDescriptorHandle& GetGpuHandle() const
		{
			return m_gpuHandle;
		}

		D3D12DescriptorHeap( Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>&& heap, uint32 increamentSize, bool bShaderVisible );
		D3D12DescriptorHeap() = default;

	protected:
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_heap;

		D3D12CpuDescriptorHandle m_cpuHandle;
		D3D12GpuDescriptorHandle m_gpuHandle;

		uint32 m_increametSize = 0;
		bool m_bShaderVisible = false;
	};

	class D3D12DescriptorHeapAllocator final
	{
	public:
		static D3D12DescriptorHeapAllocator& GetInstance()
		{
			static D3D12DescriptorHeapAllocator allocator;
			return allocator;
		}

		D3D12DescriptorHeap AllocCpuDescriptorHeap( D3D12_DESCRIPTOR_HEAP_TYPE type, uint32 num );
		D3D12DescriptorHeap AllocGpuDescriptorHeap( D3D12_DESCRIPTOR_HEAP_TYPE type, uint32 num );

		D3D12DescriptorHeapAllocator();
		D3D12DescriptorHeapAllocator( const D3D12DescriptorHeapAllocator& ) = default;
		D3D12DescriptorHeapAllocator& operator=( const D3D12DescriptorHeapAllocator& ) = default;
		D3D12DescriptorHeapAllocator( D3D12DescriptorHeapAllocator&& ) = default;
		D3D12DescriptorHeapAllocator& operator=( D3D12DescriptorHeapAllocator&& ) = default;
		~D3D12DescriptorHeapAllocator() = default;

	private:
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> AllocDescriptorHeap( D3D12_DESCRIPTOR_HEAP_TYPE type, uint32 num, D3D12_DESCRIPTOR_HEAP_FLAGS flags );

		uint32 m_descriptorHandleIncreamentSize[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES] = {};
	};
}
