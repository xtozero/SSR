#pragma once

#include "SizedTypes.h"

uint32 Crc32Hash( const void* data, uint32 size = 0, uint32 seed = 0 );
uint64 Crc64Hash( const void* data, uint32 size = 0, uint64 seed = 0 );