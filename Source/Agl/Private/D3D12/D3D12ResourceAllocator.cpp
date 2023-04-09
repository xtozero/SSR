#include "D3D12ResourceAllocator.h"

#include "D3D12Api.h"
#include "Multithread/TaskScheduler.h"

namespace agl
{
	void AllocatedResourceInfo::SetResource( ID3D12Resource* resource )
	{
		Release();
		m_resource = resource;
	}

	ID3D12Resource* AllocatedResourceInfo::GetResource() const
	{
		return m_resource;
	}

	void AllocatedResourceInfo::Release()
	{
		if ( m_resource == nullptr )
		{
			return;
		}

		if ( m_refCounter == nullptr )
		{
			m_resource->Release();
			return;
		}

		int32 refCount = m_refCounter->ReleaseRef();
		if ( refCount > 0 )
		{
			return;
		}

		EnqueueRenderTask(
			[resourceInfo = std::move( *this )]() mutable
			{
				D3D12ResourceAllocator& allocator = D3D12Allocator();
				allocator.DeallocateResource( resourceInfo );
				resourceInfo.Reset();
			} );
	}

	void AllocatedResourceInfo::Reset()
	{
		m_heap = nullptr;
		m_offset = 0;

		m_refCounter = nullptr;

		m_resource = nullptr;
	}

	bool AllocatedResourceInfo::IsExternalResource() const
	{
		return m_refCounter == nullptr;
	}

	AllocatedResourceInfo::AllocatedResourceInfo( const AllocatedHeapInfo& heapInfo, RefCounter* refCounter ) noexcept
		: AllocatedHeapInfo( heapInfo )
		, m_refCounter( refCounter )
	{
		if ( m_refCounter )
		{
			m_refCounter->AddRef();
		}
	}

	AllocatedResourceInfo::AllocatedResourceInfo( const AllocatedResourceInfo& other ) noexcept
	{
		*this = other;
	}

	AllocatedResourceInfo& AllocatedResourceInfo::operator=( const AllocatedResourceInfo& other ) noexcept
	{
		if ( this == &other )
		{
			return *this;
		}

		Release();

		m_heap = other.m_heap;
		m_offset = other.m_offset;

		m_refCounter = other.m_refCounter;
		if ( m_refCounter != nullptr )
		{
			m_refCounter->AddRef();
		}

		m_resource = other.m_resource;

		return *this;
	}

	AllocatedResourceInfo::AllocatedResourceInfo( AllocatedResourceInfo&& other ) noexcept
	{
		*this = std::move( other );
	}

	AllocatedResourceInfo& AllocatedResourceInfo::operator=( AllocatedResourceInfo&& other ) noexcept
	{
		if ( this == &other )
		{
			return *this;
		}

		Release();

		m_heap = other.m_heap;
		m_offset = other.m_offset;

		m_refCounter = other.m_refCounter;

		m_resource = other.m_resource;

		other.Reset();

		return *this;
	}

	AllocatedResourceInfo D3D12ResourceAllocator::AllocateResource( const D3D12HeapProperties& properties, const D3D12_RESOURCE_DESC& desc, D3D12_RESOURCE_STATES state, const D3D12_CLEAR_VALUE* clearValue )
	{
		D3D12_RESOURCE_ALLOCATION_INFO info = D3D12Device().GetResourceAllocationInfo( 0, 1, &desc );

		RefCounter* refCounter = m_refCounterPool.Allocate();
		std::construct_at( refCounter );

		AllocatedResourceInfo resourceInfo( m_heapAllocator.Allocate( properties, info.SizeInBytes ), refCounter );

		HRESULT hr = D3D12Device().CreatePlacedResource( resourceInfo.m_heap,
			resourceInfo.m_offset,
			&desc,
			state,
			clearValue,
			IID_PPV_ARGS( &resourceInfo.m_resource ) );

		assert( SUCCEEDED( hr ) );
		return resourceInfo;
	}

	void D3D12ResourceAllocator::DeallocateResource( const AllocatedResourceInfo& info )
	{
		if ( info.m_resource )
		{
			[[maybe_unused]] uint32 refCount = info.m_resource->Release();
			assert( refCount == 0 );
		}

		if ( info.m_heap == nullptr )
		{
			return;
		}

		m_heapAllocator.Deallocate( info );

		if ( info.m_refCounter != nullptr )
		{
			std::destroy_at( info.m_refCounter );
			m_refCounterPool.Deallocate( info.m_refCounter );
		}
	}

	D3D12ResourceAllocator::~D3D12ResourceAllocator()
	{
		assert( m_refCounterPool.Size() == 0 );
	}
}
