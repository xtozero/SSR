#pragma once

#include "SizedTypes.h"

#include <memory>
#include <type_traits>

template <typename T
	, uint32 NumInlineElements
	, typename SecondAlloc = std::allocator<T>
	, typename OriginalT = T> requires std::is_default_constructible_v<T>
class InlineAllocator
{
public:
	using value_type = T;
	using size_type = size_t;
	using difference_type = uptrint;

	[[nodiscard]] constexpr T* allocate( size_t n )
	{
		if constexpr ( std::is_same_v<T, OriginalT> )
		{
			m_size += n;
			if ( n <= NumInlineElements )
			{
				return m_inlineMemory;
			}

			return m_secondAlloc.allocate( n );
		}
		else
		{
			return std::allocator<T>().allocate( n );
		}
	}

	void deallocate( T* p, size_t n )
	{
		if constexpr ( std::is_same_v<T, OriginalT> )
		{
			m_size -= n;
			if ( IsSecondAllocatorMemoryArea( p ) )
			{
				m_secondAlloc.deallocate( p, n );
			}
		}
		else
		{
			std::allocator<T>().deallocate( p, n );
		}
	}

	void DestroyAt( T* p )
	{
		if constexpr ( std::is_same_v<T, OriginalT> )
		{
			if ( IsSecondAllocatorMemoryArea( p ) )
			{
				std::destroy_at( p );
			}
		}
		else
		{
			std::destroy_at( p );
		}
	}

	size_t GetMaxSize() const
	{
		if constexpr ( std::is_same_v<T, OriginalT> )
		{
			return ( m_size >= NumInlineElements ) ? ( std::allocator_traits<SecondAlloc>::max_size( m_secondAlloc ) ) : NumInlineElements;
		}
		else
		{
			return std::allocator_traits<std::allocator<T>>::max_size( std::allocator<T>() );
		}
	}

	constexpr InlineAllocator() noexcept = default;
	constexpr InlineAllocator( const InlineAllocator& ) noexcept = default;
	template <class Other>
	constexpr InlineAllocator( const InlineAllocator<Other, NumInlineElements, SecondAlloc>& ) noexcept {}
	~InlineAllocator() = default;
	InlineAllocator& operator=( const InlineAllocator& ) = default;

private:
	bool IsSecondAllocatorMemoryArea( const T* p ) const
	{
		return p < std::begin( m_inlineMemory ) || p >= std::end( m_inlineMemory );
	}

	size_t m_size = 0;
	T m_inlineMemory[NumInlineElements] = {};
	SecondAlloc m_secondAlloc;
};

namespace std
{
	template <typename ElementT, uint32 NumInlineElements, typename SecondAlloc>
	struct allocator_traits<InlineAllocator<ElementT, NumInlineElements, SecondAlloc>>
	{
		using allocator_type = InlineAllocator<ElementT, NumInlineElements, SecondAlloc>;
		using value_type = typename allocator_type::value_type;

		using pointer = value_type*;
		using const_pointer = const value_type*;
		using void_pointer = void*;
		using const_void_pointer = const void*;

		using size_type = size_t;
		using difference_type = uptrint;

		using propagate_on_container_copy_assignment = true_type;
		using propagate_on_container_move_assignment = true_type;
		using propagate_on_container_swap = true_type;
		using is_always_equal = false_type;

		template <typename Other>
		using rebind_alloc = InlineAllocator<Other, NumInlineElements, SecondAlloc, ElementT>;

		template <typename Other>
		using rebind_traits = allocator_traits<InlineAllocator<Other, NumInlineElements, SecondAlloc>>;

		[[nodiscard]] static constexpr pointer allocate( allocator_type& a, size_type n )
		{
			return a.allocate( n );
		}

		[[nodiscard]] static constexpr pointer allocate( allocator_type& a, size_type n, [[maybe_unused]] const_void_pointer hint )
		{
			return a.allocate( n );
		}

		static constexpr void deallocate( allocator_type& a, pointer p, size_type n )
		{
			a.deallocate( p, n );
		}

		template< class T, class... Args >
		static constexpr void construct( [[maybe_unused]] allocator_type& a, T* p, Args&&... args )
		{
			std::construct_at( p, std::forward<Args>( args )... );
		}

		template< class T >
		static constexpr void destroy( allocator_type& a, T* p )
		{
			a.DestroyAt( p );
		}

		static constexpr size_type max_size( const allocator_type& a ) noexcept
		{
			return a.GetMaxSize();
		}

		static constexpr allocator_type select_on_container_copy_construction( const allocator_type& a )
		{
			return a;
		}
	};
}
