#pragma once

#include "FixedBlockMemoryPool.h"
#include "HashUtil.h"
#include "SizedTypes.h"
#include "SparseArray.h"

#include <d3d12.h>
#include <typeinfo>
#include <unordered_map>
#include <vector>
#include <wrl/client.h>

namespace agl
{
	struct D3D12HeapProperties final
	{
		uint64 m_alignment;
		D3D12_HEAP_TYPE m_heapType;
		D3D12_HEAP_FLAGS m_heapFlags;
	};

	struct AllocatedHeapInfo
	{
		ID3D12Heap* m_heap = nullptr;
		uint64 m_offset = 0;
	};

	struct D3D12HeapSubAllocation final
	{
		uint64 m_offset = 0;
		uint64 m_size = 0;
		D3D12HeapSubAllocation* m_prev = nullptr;
		D3D12HeapSubAllocation* m_next = nullptr;
	};

	class D3D12HeapBlock final
	{
	public:
		void InitResource( uint64 size, const D3D12HeapProperties& properties );

		bool CanAllocate( uint64 size ) const;

		AllocatedHeapInfo Allocate( uint64 size );
		void Deallocate( const AllocatedHeapInfo& info );

		bool IsOwnedMemory( const AllocatedHeapInfo& info ) const;

	private:
		D3D12HeapSubAllocation* AllocateSubAllocationNode();
		void DeallocateSubAllocationNode( D3D12HeapSubAllocation* subAllocation );

		void AddToFreeList( D3D12HeapSubAllocation* prev, uint64 offset, uint64 size );
		void AddToAllocationList( uint64 offset, uint64 size );

		void RemoveFromFreeList( D3D12HeapSubAllocation& node );
		void RemoveFromAllocationList( D3D12HeapSubAllocation& node );

		D3D12HeapSubAllocation* FindFreeSpace( uint64 size ) const;
		D3D12HeapSubAllocation* FindAllocatedSpace( uint64 offset ) const;

		Microsoft::WRL::ComPtr<ID3D12Heap> m_heap;
		uint64 m_size = 0;
		uint64 m_alignment = 0;
		uint64 m_freeSize = 0;

		D3D12HeapSubAllocation* m_freeList = nullptr;
		D3D12HeapSubAllocation* m_allocationList = nullptr;

		FixedBlockMemoryPool<D3D12HeapSubAllocation> m_subAllocationNodePool;
	};

	class D3D12HeapBudget final
	{
	public:
		AllocatedHeapInfo Allocate( uint64 size );
		void Deallocate( const AllocatedHeapInfo& info );

		explicit D3D12HeapBudget( const D3D12HeapProperties& properties ) noexcept :
			m_properties( properties ) {}

	private:
		D3D12HeapBlock* FindProperHeap( uint64 size );

		static constexpr uint32 DefaultBlockSize = 64 * 1024 * 1024;

		D3D12HeapProperties m_properties;
		std::vector<D3D12HeapBlock> m_blocks;
	};

	class D3D12HeapAllocator final
	{
	public:
		AllocatedHeapInfo Allocate( const D3D12HeapProperties& properties, uint64 size );
		void Deallocate( const AllocatedHeapInfo& info );

	private:
		struct D3D12HeapPropertiesEqual
		{
			bool operator()( const D3D12HeapProperties& lhs, const D3D12HeapProperties& rhs ) const
			{
				return lhs.m_alignment == rhs.m_alignment
					&& lhs.m_heapType == rhs.m_heapType
					&& lhs.m_heapFlags == rhs.m_heapFlags;
			}
		};

		struct D3D12HeapPropertiesHasher
		{
			size_t operator()( const D3D12HeapProperties& properties ) const
			{
				static size_t typeHash = typeid( D3D12HeapProperties ).hash_code();
				size_t hash = typeHash;

				HashCombine( hash, properties.m_alignment );
				HashCombine( hash, properties.m_heapType );
				HashCombine( hash, properties.m_heapFlags );

				return hash;
			}
		};

		using HeapLookupTable = std::unordered_map<D3D12HeapProperties, D3D12HeapBudget, D3D12HeapPropertiesHasher, D3D12HeapPropertiesEqual>;

		HeapLookupTable m_heaps;
	};
}
