#include "CrcHash.h"

uint32 Crc32Hash( const void* data, uint32 size, uint32 seed )
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

uint64 Crc64Hash( const void* data, uint32 size, uint64 seed )
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