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
	std::ranges::replace( temp, '{', ' ' );
	std::ranges::replace( temp, '}', ' ' );
	RemoveSpace( temp );
	std::vector<std::string> values = SplitString( temp, ',' );

	for ( size_t i = 0; i < n && i < values.size(); ++i )
	{
		auto begin = std::begin( values[i] );
		auto end = std::end( values[i] );
		std::from_chars( std::to_address( begin ), std::to_address( end ), dest[i] );
	}
}

template <CanParseFromString T>
void Parse( T& dest, const std::string& s )
{
	T value = {};
	auto begin = std::begin( s );
	auto end = std::end( s );
	std::from_chars_result result = std::from_chars( std::to_address( begin ), std::to_address( end ), value );

	if ( result.ec == std::errc() )
	{
		dest = value;
	}
}