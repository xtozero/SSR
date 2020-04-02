#pragma once

#define NOMINMAX 1

#include "SingleLinkedList.h"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <Windows.h>

template <typename T, typename = std::enable_if_t<sizeof(T) >= sizeof(void*)>>
class FixedBlockMemoryPool
{
public:
	T* Allocate( std::size_t n )
	{
		if ( m_freeList == nullptr )
		{
			assert( false );
			return nullptr;
		}

		MemoryChunk* prevChunk = nullptr;
		MemoryChunk* tailChunk = m_freeList;
		MemoryChunk* memory = m_freeList;
		
		// 1. try allocation
		while ( memory != nullptr )
		{
			std::size_t continuousChunk = 1;
			for ( ; ( continuousChunk < n ) && ( tailChunk != nullptr );  )
			{
				char* nextChunk = reinterpret_cast<char*>( tailChunk->m_next );
				if ( nextChunk == ( reinterpret_cast<char*>( tailChunk ) + m_blockSize ) )
				{
					tailChunk = reinterpret_cast<MemoryChunk*>( nextChunk );
					++continuousChunk;
				}
				else
				{
					break;
				}
			}

			if ( continuousChunk != n )
			{
				prevChunk = tailChunk;
				memory = prevChunk->m_next;
				tailChunk = memory;
			}
			else
			{
				break;
			}
		}

		// Allocation fail
		if ( memory == nullptr )
		{
			assert( false );
			return nullptr;
		}

		if ( prevChunk == nullptr )
		{
			m_freeList = tailChunk->m_next;
		}
		else
		{
			prevChunk->m_next = tailChunk->m_next;
		}

		// 2. Return memory
		return reinterpret_cast<T*>( memory );
	}

	void Deallocate( T* memory, std::size_t n )
	{
		// 1. Initialize MemoryChunk
		MemoryChunk* entry = reinterpret_cast<MemoryChunk*>( memory );
		for ( std::size_t i = n; i > 1; --i )
		{
			entry->m_next = reinterpret_cast<MemoryChunk*>( reinterpret_cast<char*>( entry ) + m_blockSize );
			entry = entry->m_next;
		}
		entry->m_next = nullptr;

		MemoryChunk* prev = nullptr;
		MemoryChunk* memoryChunk = reinterpret_cast<MemoryChunk*>( memory );
		SLinkedList::Find( m_freeList, prev, [memoryChunk]( const MemoryChunk* other ) { return memoryChunk < other; } );

		if ( prev == nullptr )
		{
			entry->m_next = m_freeList;
			m_freeList = memoryChunk;
		}
		else 
		{
			MemoryChunk* next = prev->m_next;
			prev->m_next = memoryChunk;
			entry->m_next = next;
		}
	}

	explicit FixedBlockMemoryPool( std::size_t entryCount )
	{
		m_capacity = entryCount;
		m_blockSize = std::max( sizeof( MemoryChunk ), sizeof( T ) );
		m_poolSize = entryCount * m_blockSize;
		m_storage = VirtualAlloc( nullptr, m_poolSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE );
		assert( m_storage != nullptr );

		MemoryChunk* entry = reinterpret_cast<MemoryChunk*>( m_storage );
		for ( std::size_t i = entryCount; i > 1; --i )
		{
			entry->m_next = reinterpret_cast<MemoryChunk*>( reinterpret_cast<char*>( entry ) + m_blockSize );
			entry = entry->m_next;
		}
		entry->m_next = nullptr;

		m_freeList = reinterpret_cast<MemoryChunk*>( m_storage );
	}

	~FixedBlockMemoryPool( )
	{
		int ret = VirtualFree( m_storage, m_poolSize, MEM_DECOMMIT );
		assert( ret != 0 );
		ret = VirtualFree( m_storage, 0, MEM_RELEASE );
		assert( ret != 0 );
	}

	FixedBlockMemoryPool( const FixedBlockMemoryPool& ) = delete;
	FixedBlockMemoryPool& operator=( const FixedBlockMemoryPool& ) = delete;
	FixedBlockMemoryPool(  FixedBlockMemoryPool&& ) = delete;
	FixedBlockMemoryPool& operator=( FixedBlockMemoryPool&& ) = delete;

private:
	struct MemoryChunk
	{
		MemoryChunk* m_next;
	};

	void* m_storage = nullptr;
	std::size_t m_poolSize;
	std::size_t m_blockSize;
	std::size_t m_capacity;

	MemoryChunk* m_freeList = nullptr;
};
