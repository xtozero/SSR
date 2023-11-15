#pragma once

#include "D3D12HeapAllocator.h"

#include "FixedBlockMemoryPool.h"
#include "RefCounterType.h"

#include <set>

namespace agl
{
	class AllocatedResourceInfo final : protected AllocatedHeapInfo
	{
	public:
		void SetResource( ID3D12Resource* resource );
		ID3D12Resource* GetResource() const;

		void Release();

		void Reset();

		bool IsExternalResource() const;

		AllocatedResourceInfo( const AllocatedHeapInfo& heapInfo, RefCounter* refCounter ) noexcept;
		AllocatedResourceInfo() = default;
		AllocatedResourceInfo( const AllocatedResourceInfo& other ) noexcept;
		AllocatedResourceInfo& operator=( const AllocatedResourceInfo& other ) noexcept;
		AllocatedResourceInfo( AllocatedResourceInfo&& other ) noexcept;
		AllocatedResourceInfo& operator=( AllocatedResourceInfo&& other ) noexcept;

	private:
		friend class D3D12ResourceAllocator;

		RefCounter* m_refCounter = nullptr;
		ID3D12Resource* m_resource = nullptr;
	};

	class D3D12ResourceAllocator final
	{
	public:
		AllocatedResourceInfo AllocateResource( const D3D12HeapProperties& properties, const D3D12_RESOURCE_DESC& desc, D3D12_RESOURCE_STATES state, const D3D12_CLEAR_VALUE* clearValue = nullptr );
		void DeallocateResource( const AllocatedResourceInfo& info );

		~D3D12ResourceAllocator();

	private:
		D3D12HeapAllocator m_heapAllocator;
		FixedBlockMemoryPool<RefCounter> m_refCounterPool;
	};
}
