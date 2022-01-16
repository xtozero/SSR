#pragma once
#include "FixedBlockMemoryPool.h"

#include "SizedTypes.h"

struct MemoryPage
{
	uint8 m_data[64 * 1024];
};

class PageAllocator
{
public:
	MemoryPage* Allocate();
	void Deallocate( MemoryPage* memory );

private:
	FixedBlockMemoryPool<MemoryPage> m_allocator;
};

class StackAllocator
{
public:
	template <typename T>
	T* Allocate( size_t n )
	{
		return static_cast<T*>( Allocate( sizeof( T ) * n, alignof( T ) ) );
	}

	void* Allocate( size_t n, size_t align )
	{
		uint8* memory = reinterpret_cast<uint8*>( ( (uptrint)m_top + align - 1 ) & ~( align - 1 ) );
		uint8* newTop = memory + n;

		if ( newTop <= m_end )
		{
			m_top = newTop;
		}
		else
		{
			auto chunk = reinterpret_cast<TaggedMemory*>( m_pageAllocator.Allocate() );
			chunk->m_next = m_topChunk;
			chunk->m_size = sizeof( MemoryPage ) - sizeof( TaggedMemory );

			m_top = chunk->Data();
			m_end = m_top + chunk->m_size;
			m_topChunk = chunk;

			memory = reinterpret_cast<uint8*>( ( (uptrint)m_top + align - 1 ) & ~( align - 1 ) );
			newTop = memory + n;
			m_top = newTop;
		}

		return memory;
	}

	void Flush()
	{
		DeallocateChunks();
	}

private:
	void DeallocateChunks()
	{
		while ( m_topChunk != nullptr )
		{
			TaggedMemory* removeChunk = m_topChunk;
			m_topChunk = m_topChunk->m_next;

			m_pageAllocator.Deallocate( reinterpret_cast<MemoryPage*>( removeChunk ) );
		}

		m_top = nullptr;
		m_end = nullptr;
	}

	class TaggedMemory
	{
	public:
		TaggedMemory* m_next = nullptr;
		uint32 m_size = 0;

		uint8* Data() const
		{
			return reinterpret_cast<uint8*>( (uptrint)this + sizeof( TaggedMemory ) );
		}
	};

	uint8* m_top = nullptr;
	uint8* m_end = nullptr;
	TaggedMemory* m_topChunk = nullptr;

	PageAllocator m_pageAllocator;
};