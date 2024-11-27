#pragma once

#include <vector>
#include <string>

std::vector<std::string> SplitString( const char* str, char delim = ' ' );
std::vector<std::string> SplitString( const std::string& str, char delim = ' ');

void Trim( std::string& s );

std::string ToLower( const std::string& str );
std::string ToUpper( const std::string& str );

bool ToWideChar( wchar_t* dest, size_t destSize, const char* source );

template <typename... Args>
int32 SPrintf( char* dest, size_t destSize, const char* format, Args... args )
{
	return std::snprintf( dest, destSize, format, args... );
}