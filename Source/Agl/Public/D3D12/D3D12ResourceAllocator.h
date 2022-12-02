#pragma once

#include "D3D12HeapAllocator.h"

namespace agl
{
	struct AllocatedResourceInfo : public AllocatedHeapInfo
	{
		explicit AllocatedResourceInfo( const AllocatedHeapInfo& heapInfo ) :
			AllocatedHeapInfo( heapInfo ) {}
		AllocatedResourceInfo() = default;

		ID3D12Resource* m_resource = nullptr;
	};

	class D3D12ResourceAllocator
	{
	public:
		static D3D12ResourceAllocator& GetInstance()
		{
			static D3D12ResourceAllocator allocator;
			return allocator;
		}

		AllocatedResourceInfo AllocateResource( const D3D12HeapProperties& properties, const D3D12_RESOURCE_DESC& desc, D3D12_RESOURCE_STATES state, const D3D12_CLEAR_VALUE* clearValue = nullptr );
		void DeallocateResource( const AllocatedResourceInfo& info );

	private:
		D3D12HeapAllocator m_heapAllocator;
	};
}
