#pragma once

#include "SizedTypes.h"

#include <vector>

class BitArray
{
public:
	class BitArrayRef
	{
	public:
		BitArrayRef( uint64& bit, uint64 mask );

		operator bool() const;

		BitArrayRef& operator=( bool value );

	private:
		uint64& m_bit;
		uint64 m_mask = 0;
	};

	void Clear();

	size_t Add( bool value );

	size_t Size() const;

	void Resize( size_t newSize, bool init );

	BitArrayRef operator[]( size_t index );
	const bool operator[]( size_t index ) const;

	size_t FindFirstSetBit() const;

private:
	uint64 BitMask( size_t index ) const;

	static constexpr size_t BitSize = 64;

	std::vector<uint64> m_bits;
	size_t m_size = 0;
};

class ConstSetBitIterator
{
public:
	using iterator_category = std::forward_iterator_tag;
	using value_type = bool;
	using difference_type = std::ptrdiff_t;
	using pointer = const bool*;
	using reference = const bool&;

	ConstSetBitIterator( const BitArray& array, size_t startIndex );

	friend bool operator==( const ConstSetBitIterator& lhs, const ConstSetBitIterator& rhs );
	friend bool operator!=( const ConstSetBitIterator& lhs, const ConstSetBitIterator& rhs );

	ConstSetBitIterator& operator++();

	size_t operator*() const;

private:
	void AdvanceNextSetBit();

	const BitArray& m_array;
	size_t m_startIndex = 0;
};
