#pragma once

#include <vector>

class BitArray
{
public:
	std::size_t Add( bool value )
	{
		std::size_t index = m_bits.size( );
		m_bits.emplace_back( value );
		return index;
	}

	std::size_t Size( ) const
	{
		return m_bits.size( );
	}

	auto operator[]( std::size_t index )
	{
		return m_bits[index];
	}

	const bool operator[]( std::size_t index ) const
	{
		return m_bits[index];
	}

private:
	std::vector<bool> m_bits;
};

class ConstSetBitIterator
{
public:
	using iterator_category = std::forward_iterator_tag;
	using value_type = bool;
	using difference_type = std::ptrdiff_t;
	using pointer = const bool*;
	using reference = const bool&;

	ConstSetBitIterator( const BitArray& array, std::size_t startIndex ) : m_array( array ), m_startIndex( startIndex ) {}

	friend bool operator==( const ConstSetBitIterator& lhs, const ConstSetBitIterator& rhs )
	{
		return ( &lhs.m_array == &rhs.m_array ) && ( lhs.m_startIndex == rhs.m_startIndex );
	}

	friend bool operator!=( const ConstSetBitIterator& lhs, const ConstSetBitIterator& rhs )
	{
		return !( lhs == rhs );
	}

	ConstSetBitIterator& operator++( )
	{
		FindFirstSetBit( );
		return *this;
	}

	std::size_t operator*( ) const
	{
		return m_startIndex;
	}

private:
	void FindFirstSetBit( )
	{
		if ( m_startIndex >= m_array.Size( ) )
		{
			return;
		}

		for ( m_startIndex += 1; m_startIndex < m_array.Size( ); ++m_startIndex )
		{
			if ( m_array[m_startIndex] )
			{
				break;
			}
		}
	}

	const BitArray& m_array;
	std::size_t m_startIndex = 0;
};
