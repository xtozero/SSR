#pragma once

#include "SizedTypes.h"

constexpr uint32 Fnv1a32( const char* str, std::size_t length )
{
    constexpr uint32 offsetBasis = 2166136261u;
    constexpr uint32 prime = 16777619u;

    uint32 hash = offsetBasis;
    for ( std::size_t i = 0; i < length; ++i )
    {
        hash ^= static_cast<uint32>(str[i]);
        hash *= prime;
    }
    return hash;
}

constexpr uint32 Fnv1a32( const char* str )
{
    std::size_t len = 0;
    while ( str[len] != '\0' ) ++len;
    return Fnv1a32( str, len );
}
