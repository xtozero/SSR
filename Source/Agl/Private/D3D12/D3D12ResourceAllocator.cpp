#include "D3D12ResourceAllocator.h"

#include "D3D12Api.h"

namespace agl
{
	AllocatedResourceInfo D3D12ResourceAllocator::AllocateResource( const D3D12HeapProperties& properties, const D3D12_RESOURCE_DESC& desc, D3D12_RESOURCE_STATES state, const D3D12_CLEAR_VALUE* clearValue )
	{
		D3D12_RESOURCE_ALLOCATION_INFO info = D3D12Device().GetResourceAllocationInfo( 0, 1, &desc );

		AllocatedResourceInfo resourceInfo( m_heapAllocator.Allocate( properties, info.SizeInBytes ) );

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
			info.m_resource->Release();
		}

		m_heapAllocator.Deallocate( info );
	}
}
