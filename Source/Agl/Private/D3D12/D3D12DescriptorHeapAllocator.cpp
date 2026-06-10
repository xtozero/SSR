#include "D3D12DescriptorHeapAllocator.h"

#include "D3D12Api.h"

#include <cassert>
#include <numeric>

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

	bool D3D12DescriptorHeap::IsNull() const
	{
		return m_heap.Get() == nullptr;
	}

	void D3D12DescriptorHeap::Free()
	{
		m_heap = nullptr;
	}

	D3D12DescriptorHeap::D3D12DescriptorHeap( Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>&& heap, uint32 increamentSize, bool bShaderVisible )
		: m_heap( std::move( heap ) )
		, m_bShaderVisible( bShaderVisible )
	{
		if ( m_heap )
		{
			std::construct_at( &m_cpuHandle, m_heap->GetCPUDescriptorHandleForHeapStart(), increamentSize );

			if ( m_bShaderVisible )
			{
				std::construct_at( &m_gpuHandle, m_heap->GetGPUDescriptorHandleForHeapStart(), increamentSize );
			}
		}
	}

	D3D12DescriptorHeap D3D12DescriptorHeapAllocator::AllocCpuDescriptorHeap( D3D12_DESCRIPTOR_HEAP_TYPE type, uint32 num )
	{
		assert( 0 <= type && type < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES );
		return D3D12DescriptorHeap( AllocDescriptorHeap( type, num, D3D12_DESCRIPTOR_HEAP_FLAG_NONE ), m_descriptorHandleIncreamentSize[type], false );
	}

	D3D12DescriptorHeap D3D12DescriptorHeapAllocator::AllocGpuDescriptorHeap( D3D12_DESCRIPTOR_HEAP_TYPE type, uint32 num )
	{
		assert( 0 <= type && type < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES );
		return D3D12DescriptorHeap( AllocDescriptorHeap( type, num, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE ), m_descriptorHandleIncreamentSize[type], true );
	}

	uint32 D3D12DescriptorHeapAllocator::GetIncrementSize( D3D12_DESCRIPTOR_HEAP_TYPE type ) const
	{
		assert( 0 <= type && type < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES );
		return m_descriptorHandleIncreamentSize[type];
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

		if ( type == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER )
		{
			auto dataSize = static_cast<uint32>( std::strlen( "sampler heap" ) );
			heap->SetPrivateData( WKPDID_D3DDebugObjectName, dataSize, "sampler heap" );
		}

		return heap;
	}

	D3D12ViewDescriptorHandle D3D12ViewDescriptorPool::Acquire( D3D12_DESCRIPTOR_HEAP_TYPE type )
	{
		assert( type < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES );

		auto& heapPages = m_viewDescriptorHeapPages[type];
		D3D12ViewDescriptorPage* heapPage = nullptr;

		const auto found = std::ranges::find_if( heapPages,
			[](const std::unique_ptr<D3D12ViewDescriptorPage>& page)
			{
				return page->m_freeList.empty() == false;
			} );

		if ( found != std::end( heapPages ) )
		{
			heapPage = found->get();
		}

		if ( heapPage == nullptr )
		{
			AllocNewPage( type );
			heapPage = heapPages.back().get();
		}

		int32 freeIndex = heapPage->m_freeList.back();
		heapPage->m_freeList.pop_back();

		return {
			.m_ownerPage = heapPage,
			.m_slotIndex = freeIndex,
			.m_cpuHandle = heapPage->m_descriptorHeap.GetCpuHandle().Offset( freeIndex )
		};
	}

	void D3D12ViewDescriptorPool::Release( D3D12ViewDescriptorHandle& handle )
	{
		if ( handle.IsValid() == false )
		{
			return;
		}

		handle.m_ownerPage->m_freeList.push_back( handle.m_slotIndex );

		handle.m_ownerPage = nullptr;
		handle.m_slotIndex = -1;
	}

	void D3D12ViewDescriptorPool::AllocNewPage( D3D12_DESCRIPTOR_HEAP_TYPE type )
	{
		auto& newPage = m_viewDescriptorHeapPages[type].emplace_back( std::make_unique<D3D12ViewDescriptorPage>() );
		newPage->m_descriptorHeap = D3D12DescriptorHeapAllocator::GetInstance().AllocCpuDescriptorHeap( type, DefaultPageSize );
		newPage->m_freeList.resize( DefaultPageSize );
		std::iota( std::rbegin( newPage->m_freeList ), std::rend( newPage->m_freeList ), 0 );
	}
}
