#pragma once

#include "SizedTypes.h"

inline uint32 Crc32Hash( const void* data, uint32 size = 0, uint32 seed = 0 )
{
	static uint32 crc32LUT[256] = { 0 };
	if ( crc32LUT[1] == 0 )
	{
		constexpr uint32 polynomial = 0xEDB88320;
		for ( uint32 i = 0; i < 256; ++i )
		{
			uint32 crc = i;
			for ( uint32 j = 0; j < 8; ++j )
			{
				crc = ( crc >> 1 ) ^ ( uint32( -int32( crc & 1 ) ) & polynomial );
			}
			crc32LUT[i] = crc;
		}
	}

	seed = ~seed;
	uint32 crc = seed;
	const unsigned char* current = (const unsigned char*)data;

	if ( size > 0 )
	{
		for ( uint32 i = 0; i < size; ++i )
		{
			crc = ( crc >> 8 ) ^ crc32LUT[( crc & 0xFF ) ^ *current++];
		}
	}
	else
	{
		while ( unsigned char c = *current++ )
		{
			crc = ( crc >> 8 ) ^ crc32LUT[( crc & 0xFF ) ^ c];
		}
	}

	return ~crc;
}