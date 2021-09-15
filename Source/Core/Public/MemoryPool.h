#pragma once

#include "DoubleLinkedList.h"
#include "SizedTypes.h"

#include <cassert>
#include <cstddef>
#include <Windows.h>

class MemoryPool
{
public:
	void* Allocate( size_t size )
	{
		assert( m_freeList != nullptr );

		MemoryChunk* newChunk = nullptr;
		// 1. Try allocation
		newChunk = GetFreeChunk( size );

		// 2. Insert to allocation list
		AddAllocList( newChunk );

		// 4. Return real pointer
		return reinterpret_cast<char*>( newChunk ) + sizeof( MemoryChunk );
	}

	void Deallocate( void* ptr )
	{
		MemoryChunk* chunk = reinterpret_cast<MemoryChunk*>( static_cast<unsigned char*>( ptr ) - sizeof( MemoryChunk ) );

		// 1. Remove from allocation list
		RemoveFromAllocList( chunk );

		// 2. Insert to free list
		AddFreeList( chunk );
	}

	explicit MemoryPool( size_t poolSize ) :
		m_poolSize( poolSize + sizeof( MemoryChunk ) )
	{
		m_storage = VirtualAlloc( nullptr, m_poolSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE );
		assert( m_storage != nullptr );

		DLinkedList::Init( m_freeList, static_cast<MemoryChunk*>( m_storage ) );
		m_freeList->m_size = poolSize;
	}

	~MemoryPool( )
	{
		int32 ret = VirtualFree( m_storage, m_poolSize, MEM_DECOMMIT );
		assert( ret != 0 );
		ret = VirtualFree( m_storage, 0, MEM_RELEASE );
		assert( ret != 0 );
	}

	MemoryPool( const MemoryPool& ) = delete;
	MemoryPool& operator=( const MemoryPool& ) = delete;
	MemoryPool( MemoryPool&& ) = delete;
	MemoryPool& operator=( MemoryPool&& ) = delete;

private:
	struct MemoryChunk
	{
		MemoryChunk* m_prev;
		MemoryChunk* m_next;
		size_t m_size;
	};

	MemoryChunk* GetFreeChunk( size_t size )
	{
		MemoryChunk* chunk = DLinkedList::Find( m_freeList, [size]( const MemoryChunk* other ) { return size <= other->m_size; } );
		
		if ( chunk == nullptr )
		{
			// Allocation fail
			assert( false );
			return nullptr;
		}

		size_t remainSize = chunk->m_size - size;
		if ( remainSize >= sizeof( MemoryChunk ) )
		{
			MemoryChunk* nextChunk = reinterpret_cast<MemoryChunk*>( reinterpret_cast<unsigned char*>( chunk ) + sizeof( MemoryChunk ) + size );
			nextChunk->m_size = remainSize - sizeof( MemoryChunk );
			DLinkedList::InsertAfter( chunk, nextChunk );
		}
		else
		{
			// Memory Full
		}

		DLinkedList::Remove( m_freeList, chunk );

		chunk->m_size = size;
		chunk->m_prev = nullptr;
		chunk->m_next = nullptr;

		return chunk;
	}

	void AddAllocList( MemoryChunk* newChunk )
	{
		assert( newChunk != nullptr );

		if ( m_allocList == nullptr )
		{
			DLinkedList::Init( m_allocList, newChunk );
		}
		else
		{
			MemoryChunk* chunk = DLinkedList::Find( m_allocList, [newChunk]( const MemoryChunk* other ) { return other > newChunk; } );

			if ( chunk )
			{
				DLinkedList::InsertBefore( m_allocList, chunk, newChunk );
			}
			else
			{
				DLinkedList::AddToTail( m_allocList, newChunk );
			}
		}
	}

	void AddFreeList( MemoryChunk* freeChunk )
	{
		assert( freeChunk != nullptr );

		if ( m_freeList == nullptr )
		{
			DLinkedList::Init( m_freeList, freeChunk );
		}
		else
		{
			MemoryChunk* chunk = DLinkedList::Find( m_freeList, [freeChunk]( const MemoryChunk* other ) { return other > freeChunk; } );

			if ( chunk )
			{
				DLinkedList::InsertBefore( m_freeList, chunk, freeChunk );
			}
			else
			{
				DLinkedList::AddToTail( m_freeList, freeChunk );
			}
		}

		TryMerge( freeChunk );
	}

	void RemoveFromAllocList( MemoryChunk* elem )
	{
		MemoryChunk* chunk = DLinkedList::Find( m_allocList, [elem]( const MemoryChunk* other ) { return other == elem; } );

		if ( chunk == nullptr )
		{
			// Invalid memory pointer
			assert( false );
		}

		DLinkedList::Remove( m_allocList, chunk );

		chunk->m_prev = nullptr;
		chunk->m_next = nullptr;
	}

	void TryMerge( MemoryChunk* chunk )
	{
		assert( chunk != nullptr );

		chunk = TryMergePrev( chunk );
		TryMergeNext( chunk );
	}

	MemoryChunk* TryMergePrev( MemoryChunk* chunk )
	{
		assert( chunk != nullptr );

		MemoryChunk* prevChunk = chunk->m_prev;

		if ( prevChunk && TryMergeNext( prevChunk ) )
		{
			return prevChunk;
		}

		return chunk;
	}

	bool TryMergeNext( MemoryChunk* chunk )
	{
		assert( chunk != nullptr );

		MemoryChunk* nextChunk = reinterpret_cast<MemoryChunk*>( reinterpret_cast<unsigned char*>( chunk ) + sizeof( MemoryChunk ) + chunk->m_size );
		if ( chunk->m_next == nextChunk )
		{
			chunk->m_size += nextChunk->m_size + sizeof( MemoryChunk );
			chunk->m_next = nextChunk->m_next;
			
			if ( chunk->m_next )
			{
				chunk->m_next->m_prev = chunk;
			}

			return true;
		}

		return false;
	}

	void* m_storage = nullptr;
	size_t m_poolSize = 0;

	MemoryChunk* m_freeList = nullptr;
	MemoryChunk* m_allocList = nullptr;
};
