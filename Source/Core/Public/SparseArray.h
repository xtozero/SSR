#pragma once

#include "BitArray.h"

#include <optional>

template <typename T>
union SparseArrayElement
{
	alignas( alignof( T ) ) unsigned char m_data[sizeof( T )];

	struct SparseArrayFreeListNode
	{
		std::size_t m_prev;
		std::size_t m_next;
	} m_node;
};

template <typename T>
class SparseArray
{
public:
	std::size_t Add( const T& element )
	{
		std::size_t index = AddUninitialized( );
		new ( m_data[index].m_data )T( element );
		return index;
	}

	void RemoveAt( std::size_t index )
	{
		if ( m_allocationFlag[index] == false )
		{
			return;
		}

		Element& data = GetData( index );
		( (T&)data.m_data ).~T( );

		RemoveUninitialized( index );
	}

	T& operator[]( std::size_t index )
	{
		return ( (T&)GetData( index ).m_data );
	}

	const T& operator[]( std::size_t index ) const
	{
		return ( (const T&)GetData( index ).m_data );
	}

private:
	using Element = SparseArrayElement<T>;
	std::size_t AddUninitialized( )
	{
		std::size_t index = 0;

		if ( m_firstFreeIndex )
		{
			index = m_firstFreeIndex.value( );
			std::size_t nextIndex = GetData( index ).m_node.m_next;

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
			index = m_data.size( );
			m_data.emplace_back( );
			m_allocationFlag.Add( false );
		}

		m_allocationFlag[index] = true;
		return index;
	}

	void RemoveUninitialized( std::size_t index )
	{
		assert( m_allocationFlag[index] );
		Element& elem = GetData( index );

		elem.m_node.m_prev = index;

		if ( m_firstFreeIndex )
		{
			GetData( m_firstFreeIndex.value( ) ).m_node.m_prev = index;
		}

		elem.m_node.m_next = m_firstFreeIndex ? m_firstFreeIndex.value() : index;
		m_firstFreeIndex = index;
		m_allocationFlag[index] = false;
	}

	Element& GetData( std::size_t index )
	{
		return m_data[index];
	}

	std::vector<Element> m_data;
	BitArray m_allocationFlag;

	std::optional<std::size_t> m_firstFreeIndex;
};