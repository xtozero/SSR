#pragma once
#include "FixedBlockMemoryPool.h"

#include "SizedTypes.h"

#include <memory>

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

	constexpr StackAllocator() = default;
	constexpr StackAllocator( const StackAllocator& ) = delete;
	StackAllocator& operator=( const StackAllocator& ) = delete;
	constexpr StackAllocator( StackAllocator&& other ) noexcept
	{
		*this = std::move( other );
	}

	constexpr StackAllocator& operator=( StackAllocator&& other ) noexcept
	{
		if ( this == &other )
		{
			return *this;
		}

		DeallocateChunks();

		m_top = other.m_top;
		m_end = other.m_end;
		m_topChunk = other.m_topChunk;

		m_pageAllocator = std::move( other.m_pageAllocator );

		other.m_top = nullptr;
		other.m_end = nullptr;
		other.m_topChunk = nullptr;

		return *this;
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

template <typename T>
class TypedStackAllocator
{
public:
	using value_type = T;
	using size_type = size_t;
	using difference_type = uptrint;
	using propagate_on_container_move_assignment = std::false_type;

	[[nodiscard]] constexpr T* allocate( size_t n )
	{
		return m_allocator->Allocate<T>( n );
	}

	void deallocate( [[maybe_unused]] T* p, [[maybe_unused]] size_t n ) { /*Do Nothing*/ }

	void Flush()
	{
		m_allocator->Flush();
	}

	std::shared_ptr<StackAllocator> GetStackAllocator() const
	{
		return m_allocator;
	}

	constexpr TypedStackAllocator()
		: m_allocator( std::make_shared<StackAllocator>() )
	{
	}

	constexpr TypedStackAllocator( const TypedStackAllocator& ) = default;
	constexpr TypedStackAllocator( TypedStackAllocator&& ) = default;
	template <class Other>
	constexpr TypedStackAllocator( const TypedStackAllocator<Other>& other ) noexcept 
		: m_allocator( other.GetStackAllocator() )
	{
	}

	~TypedStackAllocator() = default;
	TypedStackAllocator& operator=( const TypedStackAllocator& ) = default;
	TypedStackAllocator& operator=( TypedStackAllocator&& ) = default;

private:
	std::shared_ptr<StackAllocator> m_allocator;
};

template <typename T1, typename T2>
constexpr bool operator==( const TypedStackAllocator<T1>& lhs, const TypedStackAllocator<T2>& rhs ) noexcept
{
	return &lhs == &rhs;
}
