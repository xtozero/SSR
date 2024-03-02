#include "BitArray.h"

#include <cmath>

BitArray::BitArrayRef::BitArrayRef( uint64& bit, uint64 mask )
	: m_bit( bit )
	, m_mask( mask )
{
}

BitArray::BitArrayRef::operator bool() const
{
	return ( m_bit & m_mask ) != 0;
}

BitArray::BitArrayRef& BitArray::BitArrayRef::operator=( bool value )
{
	if ( value )
	{
		m_bit |= m_mask;
	}
	else
	{
		m_bit &= ~m_mask;
	}

	return *this;
}

void BitArray::Clear()
{
	m_bits.clear();
	m_size = 0;
}

size_t BitArray::Add( bool value )
{
	size_t index = Size();
	++m_size;

	size_t quotient = index / BitSize;
	m_bits.resize( quotient + 1 );

	size_t remain = index % BitSize;
	m_bits[quotient] |= value ? BitMask( remain ) : 0;

	return index;
}

size_t BitArray::Size() const
{
	return m_size;
}

void BitArray::Resize( size_t newSize, bool init )
{
	if ( newSize <= Size() )
	{
		return;
	}

	constexpr uint64 allMask = std::numeric_limits<uint64>::max();

	auto newBitsSize = static_cast<size_t>( std::ceil( static_cast<double>( newSize ) / BitSize ) );
	m_bits.resize( newBitsSize, init ? allMask : 0 );

	size_t quotient = Size() / BitSize;
	size_t remain = Size() % BitSize;

	m_bits[quotient] |= init ? ( allMask >> remain ) : 0;

	m_size = newSize;
}

BitArray::BitArrayRef BitArray::operator[]( size_t index )
{
	size_t quotient = index / BitSize;
	size_t remain = index % BitSize;

	return BitArrayRef( m_bits[quotient], BitMask( remain ) );
}

const bool BitArray::operator[]( size_t index ) const
{
	size_t quotient = index / BitSize;
	size_t remain = index % BitSize;

	return ( m_bits[quotient] & BitMask( remain ) ) != 0;
}

size_t BitArray::FindFirstSetBit() const
{
	for ( size_t i = 0; i < m_bits.size(); ++i )
	{
		if ( m_bits[i] == false )
		{
			continue;
		}

		for ( size_t j = 0; j < BitSize; ++j )
		{
			uint64 mask = BitMask( j );
			if ( ( m_bits[i] & mask ) != 0 )
			{
				size_t index = i * BitSize + j;
				return index;
			}
		}
	}

	return Size();
}

uint64 BitArray::BitMask( size_t index ) const
{
	return 1ui64 << ( BitSize - 1 - index );
}

ConstSetBitIterator::ConstSetBitIterator( const BitArray& array, size_t startIndex )
	: m_array( array )
	, m_startIndex( startIndex )
{
}

bool operator==( const ConstSetBitIterator& lhs, const ConstSetBitIterator& rhs )
{
	return ( &lhs.m_array == &rhs.m_array ) && ( lhs.m_startIndex == rhs.m_startIndex );
}

bool operator!=( const ConstSetBitIterator& lhs, const ConstSetBitIterator& rhs )
{
	return !( lhs == rhs );
}

ConstSetBitIterator& ConstSetBitIterator::operator++()
{
	AdvanceNextSetBit();
	return *this;
}

size_t ConstSetBitIterator::operator*() const
{
	return m_startIndex;
}

void ConstSetBitIterator::AdvanceNextSetBit()
{
	if ( m_startIndex >= m_array.Size() )
	{
		return;
	}

	for ( m_startIndex += 1; m_startIndex < m_array.Size(); ++m_startIndex )
	{
		if ( m_array[m_startIndex] )
		{
			break;
		}
	}
}
