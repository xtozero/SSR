#pragma once

#include "SizedTypes.h"

inline uint64 Crc64Hash( const void* data, uint32 size = 0, uint64 seed = 0 )
{
	static uint64 crc64LUT[256] = {};
	if ( crc64LUT[1] == 0 )
	{
		constexpr uint64 polynomial = 0xC96C5795D7870F42;
		for ( uint32 i = 0; i < 256; ++i )
		{
			uint64 crc = i;
			for ( uint32 j = 0; j < 8; ++j )
			{
				crc = ( crc >> 1 ) ^ ( uint64( -int64( crc & 1 ) ) & polynomial );
			}
			crc64LUT[i] = crc;
		}
	}

	seed = ~seed;
	uint64 crc = seed;
	const unsigned char* current = (const unsigned char*)data;

	if ( size > 0 )
	{
		for ( uint32 i = 0; i < size; ++i )
		{
			crc = ( crc >> 8 ) ^ crc64LUT[( crc & 0xFF ) ^ *current++];
		}
	}
	else
	{
		while ( unsigned char c = *current++ )
		{
			crc = ( crc >> 8 ) ^ crc64LUT[( crc & 0xFF ) ^ c];
		}
	}

	return ~crc;
}