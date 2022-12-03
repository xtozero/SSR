#include "D3D12DescriptorHeapAllocator.h"

#include "D3D12Api.h"

#include <cassert>

namespace agl
{
	D3D12_CPU_DESCRIPTOR_HANDLE D3D12CpuDescriptorHandle::At( int32 offset ) const
	{
		D3D12_CPU_DESCRIPTOR_HANDLE handle = m_handle;
		handle.ptr += static_cast<int64>( offset ) * m_increametSize;

		return handle;
	}

	D3D12CpuDescriptorHandle D3D12CpuDescriptorHandle::Offset( int32 offset ) const
	{
		return D3D12CpuDescriptorHandle( At( offset ), m_increametSize );
	}

	D3D12CpuDescriptorHandle::D3D12CpuDescriptorHandle( D3D12_CPU_DESCRIPTOR_HANDLE handle, uint32 increametSize )
		: D3D12DescriptorHandle( increametSize )
		, m_handle( handle )
	{
	}

	D3D12_GPU_DESCRIPTOR_HANDLE D3D12GpuDescriptorHandle::At( int32 offset ) const
	{
		D3D12_GPU_DESCRIPTOR_HANDLE handle = m_handle;
		handle.ptr += static_cast<int64>( offset ) * m_increametSize;

		return handle;
	}

	D3D12GpuDescriptorHandle D3D12GpuDescriptorHandle::Offset( int32 offset ) const
	{
		return D3D12GpuDescriptorHandle( At( offset ), m_increametSize );
	}

	D3D12GpuDescriptorHandle::D3D12GpuDescriptorHandle( D3D12_GPU_DESCRIPTOR_HANDLE handle, uint32 increametSize )
		: D3D12DescriptorHandle( increametSize )
		, m_handle( handle )
	{
	}

	D3D12DescriptorHeap::D3D12DescriptorHeap( Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>&& heap, uint32 increamentSize, bool bShaderVisible )
		: m_heap( std::move( heap ) )
		, m_increametSize( increamentSize )
		, m_bShaderVisible( bShaderVisible )
	{
		if ( m_heap )
		{
			std::construct_at( &m_cpuHandle, m_heap->GetCPUDescriptorHandleForHeapStart(), m_increametSize );

			if ( m_bShaderVisible )
			{
				std::construct_at( &m_gpuHandle, m_heap->GetGPUDescriptorHandleForHeapStart(), m_increametSize );
			}
		}
	}

	D3D12DescriptorHeap D3D12DescriptorHeapAllocator::AllocCpuDescriptorHeap( D3D12_DESCRIPTOR_HEAP_TYPE type, uint32 num )
	{
		return D3D12DescriptorHeap( AllocDescriptorHeap( type, num, D3D12_DESCRIPTOR_HEAP_FLAG_NONE ), m_descriptorHandleIncreamentSize[type], false );
	}

	D3D12DescriptorHeap D3D12DescriptorHeapAllocator::AllocGpuDescriptorHeap( D3D12_DESCRIPTOR_HEAP_TYPE type, uint32 num )
	{
		return D3D12DescriptorHeap( AllocDescriptorHeap( type, num, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE ), m_descriptorHandleIncreamentSize[type], true );
	}

	D3D12DescriptorHeapAllocator::D3D12DescriptorHeapAllocator()
	{
		for ( uint32 i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i )
		{
			m_descriptorHandleIncreamentSize[i] = D3D12Device().GetDescriptorHandleIncrementSize( static_cast<D3D12_DESCRIPTOR_HEAP_TYPE>( i ) );
		}
	}

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> D3D12DescriptorHeapAllocator::AllocDescriptorHeap( D3D12_DESCRIPTOR_HEAP_TYPE type, uint32 num, D3D12_DESCRIPTOR_HEAP_FLAGS flags )
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {
			.Type = type,
			.NumDescriptors = num,
			.Flags = flags,
			.NodeMask = 0
		};

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> heap;
		[[maybe_unused]] HRESULT hr = D3D12Device().CreateDescriptorHeap( &desc, IID_PPV_ARGS( heap.GetAddressOf() ) );
		assert( SUCCEEDED( hr ) );

		return heap;
	}
}
