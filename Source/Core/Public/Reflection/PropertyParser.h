#pragma once

#include "SizedTypes.h"
#include "StringUtility.h"

#include <algorithm>
#include <charconv>

template <typename T>
concept CanParseFromString = requires( T a )
{
	std::from_chars( nullptr, nullptr, a );
};

template <typename T>
void ParseArray( [[maybe_unused]] T* dest, [[maybe_unused]] size_t n, [[maybe_unused]] const std::string& s ) {}

template <typename T>
void Parse( [[maybe_unused]] T& dest, [[maybe_unused]] const std::string& s ) {}

template <CanParseFromString T>
void ParseArray( T* dest, size_t n, const std::string& s ) 
{
	std::string temp = s;
	std::replace( std::begin( temp ), std::end( temp ), '{', ' ' );
	std::replace( std::begin( temp ), std::end( temp ), '}', ' ' );
	Trim( temp );
	std::vector<std::string> values = SplitString( temp, ',' );

	for ( size_t i = 0; i < n && i < values.size(); ++i )
	{
		Parse( *dest++, values[i] );
	}
}

template <CanParseFromString T>
void Parse( T& dest, const std::string& s )
{
	T value = {};
	std::from_chars_result result = std::from_chars( s.data(), s.data() + s.size(), value );

	if ( result.ec == std::errc() )
	{
		dest = value;
	}
}