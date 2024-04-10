#pragma once

#include "SizedTypes.h"

#include <algorithm>

template <typename T>
using Owner = T;

template <uint32 N>
struct StringLiteral
{
	constexpr StringLiteral( const char( &str )[N] )
	{
		std::copy_n( str, N, m_str );
	}

	char m_str[N] = {};
};

using float2 = float[2];
using float3 = float[3];
using float4 = float[4];