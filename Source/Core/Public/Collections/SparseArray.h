#pragma once

#include "BitArray.h"

#include <cassert>
#include <optional>
#include <type_traits>

template <typename T>
union SparseArrayElement
{
	alignas( alignof( T ) ) unsigned char m_data[sizeof( T )];

	struct SparseArrayFreeListNode
	{
		size_t m_prev;
		size_t m_next;
	} m_node;
};

template <typename T>
class SparseArray
{
public:
	void Clear()
	{
		for ( auto& data : *this )
		{
			( (T&)data ).~T();
		}

		m_data.clear();
		m_allocationFlag.Clear();

		m_firstFreeIndex = {};
		m_size = 0;
	}

	size_t AddUninitialized()
	{
		size_t index = 0;

		if ( m_firstFreeIndex )
		{
			index = m_firstFreeIndex.value();
			size_t nextIndex = GetData( index ).m_node.m_next;

			if ( nextIndex != index )
			{
				GetData( nextIndex ).m_node.m_prev = nextIndex;
				m_firstFreeIndex = nextIndex;
			}
			else
			{
				m_firstFreeIndex = std::nullopt;
			}
		}
		else
		{
			index = m_data.size();
			m_data.emplace_back();
			m_allocationFlag.Add( false );
		}

		++m_size;
		m_allocationFlag[index] = true;
		return index;
	}

	size_t AddZeroed()
	{
		size_t index = AddUninitialized();
		std::construct_at( reinterpret_cast<T*>( &m_data[index].m_data ), T{} );

		return index;
	}

	template <typename U>
	size_t Add( U&& element )
	{
		size_t index = AddUninitialized();
		std::construct_at( reinterpret_cast<T*>( &m_data[index].m_data ), std::forward<U>( element ) );

		return index;
	}

	void RemoveAt( size_t index )
	{
		if ( m_allocationFlag[index] == false )
		{
			return;
		}

		Element& data = GetData( index );
		( (T&)data.m_data ).~T();

		RemoveUninitialized( index );
	}

	void Remove( const T& element )
	{
		auto found = Find( element );
		if ( found != end() )
		{
			RemoveAt( found.Index() );
		}
	}

	size_t Size() const
	{
		return m_size;
	}

	size_t GetMaxIndex() const
	{
		return m_data.size();
	}

	bool IsAllocated( size_t index ) const
	{
		return m_allocationFlag[index];
	}

	T& operator[]( size_t index )
	{
		return ( (T&)GetData( index ).m_data );
	}

	const T& operator[]( size_t index ) const
	{
		return ( (const T&)GetData( index ).m_data );
	}

	template <bool IsConst>
	class IteratorBase
	{
	public:
		using iterator_category = std::forward_iterator_tag;
		using value_type = T;
		using difference_type = std::ptrdiff_t;
		using pointer = std::conditional_t<IsConst, const value_type*, value_type*>;
		using reference = std::conditional_t<IsConst, const value_type&, value_type&>;

		using ArrayType = std::conditional_t<IsConst, const SparseArray, SparseArray>;

		size_t Index() const
		{
			return *m_bitIter;
		}

		IteratorBase& operator++()
		{
			++m_bitIter;
			return *this;
		}

		reference operator*() const
		{
			return m_array[*m_bitIter];
		}

		friend bool operator==( const IteratorBase& lhs, const IteratorBase& rhs )
		{
			return ( &lhs.m_array == &rhs.m_array ) && ( lhs.m_bitIter == rhs.m_bitIter );
		}

		friend bool operator!=( const IteratorBase& lhs, const IteratorBase& rhs )
		{
			return !( lhs == rhs );
		}

		IteratorBase( ArrayType& array, size_t startIndex ) : m_array( array ), m_bitIter( ConstSetBitIterator( array.m_allocationFlag, startIndex ) )
		{}

	protected:
		ArrayType& m_array;
		ConstSetBitIterator m_bitIter;
	};

	class Iterator : public IteratorBase<false>
	{
	public:
		using IteratorBase<false>::IteratorBase;
	};

	class ConstIterator : public IteratorBase<true>
	{
	public:
		using IteratorBase<true>::IteratorBase;
	};

	Iterator begin() noexcept
	{
		return Iterator( *this, m_allocationFlag.FindFirstSetBit() );
	}

	ConstIterator begin() const noexcept
	{
		return ConstIterator( *this, m_allocationFlag.FindFirstSetBit() );
	}

	Iterator end() noexcept
	{
		return Iterator( *this, m_allocationFlag.Size() );
	}

	ConstIterator end() const noexcept
	{
		return ConstIterator( *this, m_allocationFlag.Size() );
	}

	Iterator Find( const T& element )
	{
		for ( auto iter = begin(); iter != end(); ++iter )
		{
			if ( *iter == element )
			{
				return iter;
			}
		}

		return end();
	}

private:
	using Element = SparseArrayElement<T>;

	void RemoveUninitialized( size_t index )
	{
		assert( m_allocationFlag[index] );
		Element& elem = GetData( index );

		elem.m_node.m_prev = index;

		if ( m_firstFreeIndex )
		{
			GetData( m_firstFreeIndex.value() ).m_node.m_prev = index;
		}

		elem.m_node.m_next = m_firstFreeIndex ? m_firstFreeIndex.value() : index;
		m_firstFreeIndex = index;

		--m_size;
		m_allocationFlag[index] = false;
	}

	Element& GetData( size_t index )
	{
		return m_data[index];
	}

	const Element& GetData( size_t index ) const
	{
		return m_data[index];
	}

	std::vector<Element> m_data;
	BitArray m_allocationFlag;

	std::optional<size_t> m_firstFreeIndex;

	size_t m_size = 0;
};