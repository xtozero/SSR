#include "D3D12HeapAllocator.h"

#include "D3D12Api.h"
#include "DoubleLinkedList.h"
#include "Math/Util.h"
#include "TaskScheduler.h"

#include <cassert>

namespace agl
{
	void D3D12HeapBlock::InitResource( uint64 size, const D3D12HeapProperties& properties )
	{
		assert( IsInRenderThread() );

		m_size = size;
		m_alignment = properties.m_alignment;
		m_freeSize = m_size;

		D3D12_HEAP_DESC heapDesc = {
			.SizeInBytes = m_size,
			.Properties = {
				.Type = properties.m_heapType,
				.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
				.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN,
				.CreationNodeMask = 0,
				.VisibleNodeMask = 0
			},
			.Alignment = properties.m_alignment,
			.Flags = properties.m_heapFlags
		};

		HRESULT hr = D3D12Device().CreateHeap( &heapDesc, IID_PPV_ARGS( m_heap.GetAddressOf() ) );

		assert( SUCCEEDED( hr ) );

		AddToFreeList( nullptr, 0, m_size );
	}

	bool D3D12HeapBlock::CanAllocate( uint64 size ) const
	{
		assert( IsInRenderThread() );

		uint64 alignedSize = CalcAlignment( size, m_alignment );
		return FindFreeSpace( alignedSize ) != nullptr;
	}

	AllocatedHeapInfo D3D12HeapBlock::Allocate( uint64 size )
	{
		assert( IsInRenderThread() );

		uint64 alignedSize = CalcAlignment( size, m_alignment );
		D3D12HeapSubAllocation* node = FindFreeSpace( alignedSize );
		if ( node == nullptr )
		{
			return {};
		}

		AllocatedHeapInfo allocated = {
			.m_heap = m_heap.Get(),
			.m_offset = node->m_offset
		};

		m_freeSize -= alignedSize;

		AddToAllocationList( node->m_offset, alignedSize );

		uint64 freeMemoryOffset = node->m_offset + alignedSize;
		uint64 freeMemorySize = node->m_size - alignedSize;
		if ( freeMemorySize > 0 )
		{
			AddToFreeList( node, freeMemoryOffset, freeMemorySize );
		}

		RemoveFromFreeList( *node );

		assert( ValidateFreeList() );

		return allocated;
	}

	void D3D12HeapBlock::Deallocate( const AllocatedHeapInfo& info )
	{
		assert( IsInRenderThread() );

		D3D12HeapSubAllocation* node = FindAllocatedSpace( info.m_offset );
		if ( node == nullptr )
		{
			return;
		}

		m_freeSize += node->m_size;

		D3D12HeapSubAllocation* prevNode = DLinkedList::Find( m_freeList,
			[&info]( const D3D12HeapSubAllocation* node )
			{
				if ( node->m_next == nullptr )
				{
					return node->m_offset < info.m_offset;
				}
				else
				{
					const D3D12HeapSubAllocation* next = node->m_next;
					return ( node->m_offset < info.m_offset ) && ( info.m_offset < next->m_offset ) ;
				}
			});

		AddToFreeList( prevNode, node->m_offset, node->m_size );

		D3D12HeapSubAllocation* iter = ( prevNode == nullptr ) ? m_freeList : prevNode;
		while ( iter != nullptr )
		{
			D3D12HeapSubAllocation* iterNext = iter->m_next;
			if ( iterNext == nullptr )
			{
				break;
			}

			if ( ( iter->m_offset + iter->m_size ) != iterNext->m_offset )
			{
				break;
			}

			iter->m_size += iterNext->m_size;
			RemoveFromFreeList( *iterNext );
		}
		
		RemoveFromAllocationList( *node );

		assert( ValidateFreeList() );
	}

	bool D3D12HeapBlock::IsOwnedMemory( const AllocatedHeapInfo& info ) const
	{
		return info.m_heap == m_heap.Get();
	}

	D3D12HeapSubAllocation* D3D12HeapBlock::AllocateSubAllocationNode()
	{
		assert( IsInRenderThread() );

		D3D12HeapSubAllocation* subAllocation = m_subAllocationNodePool.Allocate();
		std::construct_at( subAllocation );

		return subAllocation;
	}

	void D3D12HeapBlock::DeallocateSubAllocationNode( D3D12HeapSubAllocation* subAllocation )
	{
		assert( IsInRenderThread() );

		std::destroy_at( subAllocation );
		m_subAllocationNodePool.Deallocate( subAllocation );
	}

	void D3D12HeapBlock::AddToFreeList( D3D12HeapSubAllocation* prev, uint64 offset, uint64 size )
	{
		D3D12HeapSubAllocation* node = AllocateSubAllocationNode();
		node->m_offset = offset;
		node->m_size = size;

		if ( prev == nullptr )
		{
			if ( m_freeList == nullptr )
			{
				DLinkedList::Init( m_freeList, node );
			}
			else
			{
				DLinkedList::InsertBefore( m_freeList, m_freeList, node );
			}
		}
		else
		{
			DLinkedList::InsertAfter( prev, node );
		}
	}

	void D3D12HeapBlock::AddToAllocationList( uint64 offset, uint64 size )
	{
		D3D12HeapSubAllocation* node = AllocateSubAllocationNode();
		node->m_offset = offset;
		node->m_size = size;

		if ( m_allocationList == nullptr )
		{
			DLinkedList::Init( m_allocationList, node );
		}
		else
		{
			DLinkedList::InsertAfter( m_allocationList, node );
		}
	}

	void D3D12HeapBlock::RemoveFromFreeList( D3D12HeapSubAllocation& node )
	{
		DLinkedList::Remove( m_freeList, &node );
		DeallocateSubAllocationNode( &node );
	}

	void D3D12HeapBlock::RemoveFromAllocationList( D3D12HeapSubAllocation& node )
	{
		DLinkedList::Remove( m_allocationList, &node );
		DeallocateSubAllocationNode( &node );
	}

	D3D12HeapSubAllocation* D3D12HeapBlock::FindFreeSpace( uint64 size ) const
	{
		D3D12HeapSubAllocation* found = DLinkedList::Find( m_freeList, 
			[size]( const D3D12HeapSubAllocation* node )
			{
				return size <= node->m_size;
			});

		return found;
	}

	D3D12HeapSubAllocation* D3D12HeapBlock::FindAllocatedSpace( uint64 offset ) const
	{
		D3D12HeapSubAllocation* found = DLinkedList::Find( m_allocationList,
			[offset]( const D3D12HeapSubAllocation* node )
			{
				return node->m_offset == offset;
			} );

		return found;
	}

	bool D3D12HeapBlock::ValidateFreeList()
	{
		D3D12HeapSubAllocation* head = m_freeList;

		while ( head )
		{
			if ( head->m_next )
			{
				if ( head->m_offset > head->m_next->m_offset )
				{
					return false;
				}
			}

			head = head->m_next;
		}

		return true;
	}

	AllocatedHeapInfo D3D12HeapBudget::Allocate( uint64 size )
	{
		assert( IsInRenderThread() );

		D3D12HeapBlock* block = FindProperHeap( size );
		if ( block == nullptr )
		{
			block = &m_blocks.emplace_back();
			block->InitResource( std::max<uint64>( DefaultBlockSize, size ), m_properties );
		}

		return block->Allocate( size );
	}

	void D3D12HeapBudget::Deallocate( const AllocatedHeapInfo& info )
	{
		assert( IsInRenderThread() );

		for ( D3D12HeapBlock& block : m_blocks )
		{
			if ( block.IsOwnedMemory( info ) )
			{
				block.Deallocate( info );
				return;
			}
		}
	}

	D3D12HeapBlock* D3D12HeapBudget::FindProperHeap( uint64 size )
	{
		for ( D3D12HeapBlock& block : m_blocks )
		{
			if ( block.CanAllocate( size ) )
			{
				return &block;
			}
		}

		return nullptr;
	}

	AllocatedHeapInfo D3D12HeapAllocator::Allocate( const D3D12HeapProperties& properties, uint64 size )
	{
		assert( IsInRenderThread() );

		auto iter = m_heaps.find( properties );
		if ( iter == std::end( m_heaps ) )
		{
			iter = m_heaps.emplace( properties, properties ).first;
		}
		
		D3D12HeapBudget& heapBudget = iter->second;
		return heapBudget.Allocate( size );
	}

	void D3D12HeapAllocator::Deallocate( const AllocatedHeapInfo& info )
	{
		assert( IsInRenderThread() );

		D3D12_HEAP_DESC desc = info.m_heap->GetDesc();
		D3D12HeapProperties properties = {
			.m_alignment = desc.Alignment,
			.m_heapType = desc.Properties.Type,
			.m_heapFlags = desc.Flags
		};

		auto iter = m_heaps.find( properties );
		if ( iter == std::end( m_heaps ) )
		{
			return;
		}

		D3D12HeapBudget& heapBudget = iter->second;
		return heapBudget.Deallocate( info );
	}
}
