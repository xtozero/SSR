#include "Halton.h"

#include <cassert>
#include <cmath>

namespace
{
	uint32 ReverseBits( uint32 n )
	{
		n = ( n << 16 ) | ( n >> 16 );
		n = ( ( n & 0x00ff00ff ) << 8 ) | ( ( n & 0xff00ff00 ) >> 8 );
		n = ( ( n & 0x0f0f0f0f ) << 4 ) | ( ( n & 0xf0f0f0f0 ) >> 4 );
		n = ( ( n & 0x33333333 ) << 2 ) | ( ( n & 0xcccccccc ) >> 2 );
		n = ( ( n & 0x55555555 ) << 1 ) | ( ( n & 0xaaaaaaaa ) >> 1 );
		return n;
	}

	uint64 ReverseBits( uint64 n )
	{
		uint64 hi = ReverseBits( static_cast<uint32>( n ) );
		uint64 lo = ReverseBits( static_cast<uint32>( n >> 32 ) );
		return ( hi << 32 ) | lo;
	}

	template <uint64 base>
	float RadicalInverse( uint64 a )
	{
		constexpr float invBase = 1.f / base;
		uint64 reversedDigits = 0;
		float invBaseN = 1;

		while ( a )
		{
			uint64 next = a / base;
			uint64 digit = a % base;
			reversedDigits = reversedDigits * base + digit;
			invBaseN *= invBase;
			a = next;
		}

		return std::min( reversedDigits * invBaseN, (float)0x1.fffffep-1 );
	}
}

float Halton::Sample( uint64 index, uint32 dim ) const
{
	switch ( dim )
	{
	case 0:
	case 1:
	case 2:
		return RadicalInverse( dim, index );
	default:
		assert( false );
		break;
	}

	return 0.5f;
}

Vector2 Halton::Sample2D( uint64 index ) const
{
	return Vector2( Sample( index, 0 ), Sample( index, 1 ) );
}

Vector Halton::Sample3D( uint64 index ) const
{
	return Vector( Sample( index, 0 ), Sample( index, 1 ), Sample( index, 2 ) );
}

float Halton::RadicalInverse( uint32 baseIndex, uint64 a ) const
{
	switch ( baseIndex )
	{
	case 0:
		return static_cast<float>( ReverseBits( a ) * 0x1p-64 );
	case 1:
		return ::RadicalInverse<3>( a );
	case 2:
		return ::RadicalInverse<5>( a );
	default:
		assert( false );
		break;
	}

	return 0.5f;
}
