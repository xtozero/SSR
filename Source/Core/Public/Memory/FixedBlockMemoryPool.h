#pragma once

#define NOMINMAX 1

#include "SingleLinkedList.h"
#include "SizedTypes.h"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <Windows.h>

template <typename T>
class FixedBlockMemoryPool
{
public:
	T* Allocate( size_t n )
	{
		if ( n == 1 )
		{
			if ( m_freeList == nullptr )
			{
				AllocateChunk( m_nextChunkSize );
				m_nextChunkSize <<= 1;
			}
		}
		else
		{
			AllocateChunk( n );
			if ( m_nextChunkSize < n )
			{
				m_nextChunkSize = n << 1;
			}
		}

		MemoryBlock* result = m_freeList;
		for ( size_t i = 0; i < n; ++i )
		{
			MemoryBlock* memory = m_freeList;
			if ( memory == nullptr )
			{
				assert( false );
				return nullptr;
			}

			++m_size;
			SLinkedList::Remove( m_freeList, memory );
		}

		return reinterpret_cast<T*>( result );
	}

	void Deallocate( T* memory, size_t n )
	{
		for ( size_t i = 0; i < n; ++i )
		{
			auto block = reinterpret_cast<MemoryBlock*>( memory + i );
			block->m_next = nullptr;

			if ( m_freeList == nullptr )
			{
				m_freeList = block;
			}
			else
			{
				SLinkedList::AddToTail( m_freeList, block );
			}

			--m_size;
		}
	}

	FixedBlockMemoryPool() = default;
	~FixedBlockMemoryPool()
	{
		while ( m_chunkList )
		{
			DeallocateChunk( m_chunkList );
		}
	}

	FixedBlockMemoryPool( const FixedBlockMemoryPool& ) = delete;
	FixedBlockMemoryPool& operator=( const FixedBlockMemoryPool& ) = delete;
	FixedBlockMemoryPool( FixedBlockMemoryPool&& ) = delete;
	FixedBlockMemoryPool& operator=( FixedBlockMemoryPool&& ) = delete;

private:
	struct MemoryBlock
	{
		MemoryBlock* m_next;
	};

	struct MemoryChunk
	{
		MemoryChunk* m_next;
		size_t m_size = 0;
	};

	void* AllocateChunk( size_t entryCount )
	{
		size_t chunkSize = sizeof( MemoryChunk ) + BlockSize * entryCount;
		auto chunk = reinterpret_cast<MemoryChunk*>( VirtualAlloc( nullptr, chunkSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE ) );
		chunk->m_size = chunkSize;

		void* block = reinterpret_cast<void*>( ( (uptrint)chunk ) + sizeof( MemoryChunk ) );
		MemoryBlock* head = reinterpret_cast<MemoryBlock*>( block );
		MemoryBlock* entry = head;
		for ( size_t i = 1; i < entryCount; ++i )
		{
			auto next = reinterpret_cast<MemoryBlock*>( ( (uptrint)entry ) + BlockSize );
			SLinkedList::InsertAfter( entry, next );
			entry = entry->m_next;
		}
		entry->m_next = nullptr;

		m_capacity += entryCount;

		if ( m_freeList == nullptr )
		{
			SLinkedList::Init( m_freeList, head );
		}
		else
		{
			SLinkedList::AddToHead( m_freeList, head );
		}

		if ( m_chunkList == nullptr )
		{
			SLinkedList::Init( m_chunkList, chunk );
		}
		else
		{
			SLinkedList::AddToHead( m_chunkList, chunk );
		}

		return head;
	}

	void DeallocateChunk( MemoryChunk* chunk )
	{
		SLinkedList::Remove( m_chunkList, chunk );

		int32 ret = VirtualFree( chunk, chunk->m_size, MEM_DECOMMIT );
		assert( ret != 0 );
		ret = VirtualFree( chunk, 0, MEM_RELEASE );
		assert( ret != 0 );
	}

	MemoryChunk* m_chunkList = nullptr;
	size_t m_capacity = 0;
	size_t m_size = 0;
	size_t m_nextChunkSize = 1;

	MemoryBlock* m_freeList = nullptr;

	static constexpr size_t BlockSize = std::max( sizeof( MemoryBlock ), sizeof( T ) );
};
