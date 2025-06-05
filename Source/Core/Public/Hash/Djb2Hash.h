#pragma once

#include "SizedTypes.h"

constexpr uint64 Djb2DefaultSeed = 5381;

inline uint64 Djb2Hash( const char* data, size_t length, uint64 seed )
{
    uint64 hash = seed;
    for ( size_t i = 0; i < length; ++i )
    {
        hash = ( ( hash << 5 ) + hash ) + data[i]; // hash * 33 + data[i]
    }
    return hash;
}

inline uint64 Djb2Hash( const char* data, size_t length )
{
    return Djb2Hash( data, length, Djb2DefaultSeed );
}
