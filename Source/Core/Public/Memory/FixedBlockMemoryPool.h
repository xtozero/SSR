#pragma once

#define NOMINMAX 1

#include "Math/Util.h"
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
	T* Allocate()
	{
		if ( ( m_capacity - m_size ) == 0 )
		{
			AllocateChunk( m_nextChunkSize );
			m_nextChunkSize <<= 1;
		}

		MemoryBlock* memory = m_freeList;
		if ( memory == nullptr )
		{
			assert( false );
			return nullptr;
		}

		++m_size;
		SLinkedList::Remove( m_freeList, memory );

		return reinterpret_cast<T*>( memory );
	}

	void Deallocate( T* memory )
	{
		auto block = reinterpret_cast<MemoryBlock*>( memory );
		block->m_next = nullptr;

		if ( m_freeList == nullptr )
		{
			SLinkedList::Init( m_freeList, block );
		}
		else
		{
			SLinkedList::AddToTail( m_freeList, block );
		}

		--m_size;
	}

	size_t Size() const
	{
		return m_size;
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
	FixedBlockMemoryPool( FixedBlockMemoryPool&& other ) noexcept
	{
		*this = std::move( other );
	}

	FixedBlockMemoryPool& operator=( FixedBlockMemoryPool&& other ) noexcept
	{
		if ( this == &other )
		{
			return *this;
		}

		m_chunkList = other.m_chunkList;
		m_capacity = other.m_capacity;
		m_size = other.m_size;
		m_nextChunkSize = other.m_nextChunkSize;
		m_freeList = other.m_freeList;

		other.m_chunkList = nullptr;
		other.m_capacity = 0;
		other.m_size = 0;
		other.m_nextChunkSize = CalcAlignment<size_t>( BlockSize, ChunkAlignment );
		other.m_freeList = nullptr;

		return *this;
	}

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

	void* AllocateChunk( size_t chunkSize )
	{
		chunkSize = CalcAlignment<size_t>( chunkSize, ChunkAlignment );
		size_t entryCount = ( chunkSize - sizeof( MemoryChunk ) ) / BlockSize;

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
	size_t m_nextChunkSize = CalcAlignment<size_t>( BlockSize, ChunkAlignment );

	MemoryBlock* m_freeList = nullptr;

	static constexpr size_t BlockSize = std::max( sizeof( MemoryBlock ), sizeof( T ) );
	static constexpr size_t ChunkAlignment = 4096;
};
