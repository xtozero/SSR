#pragma once

#include <cassert>
#include <cstddef>
#include <sstream>
#include <tchar.h>
#include <Windows.h>
#include <vector>

#define stdHandle GetStdHandle( STD_OUTPUT_HANDLE )
#define SetColor_Red SetConsoleTextAttribute( stdHandle, 0x000c )
#define SetColor_Blue SetConsoleTextAttribute( stdHandle, 0x0001 | 0x0008 )
#define SetColor_Green SetConsoleTextAttribute( stdHandle, 0x0001 | 0x0008 )
#define SetColor_White SetConsoleTextAttribute( stdHandle, 0x000f )
#define SetColor_Skyblue SetConsoleTextAttribute( stdHandle, 0x000b )
#define SetColor_Yellow SetConsoleTextAttribute( stdHandle, 0x000e )
#define SetColor_Chemical SetConsoleTextAttribute( stdHandle, 0x000d )
#define SetColor_Gold SetConsoleTextAttribute( stdHandle, 0x0006 )
#define SetColor_Wine SetConsoleTextAttribute( stdHandle, 0x0005 )
#define SetColor_Blood SetConsoleTextAttribute( stdHandle, 0x0004 )
#define SetColor_Violet SetConsoleTextAttribute( stdHandle, 0x0001 | 0x0008 | 0x000c )

namespace UTIL
{
	inline void Split( const std::string& string, std::vector<std::string>& params, const char token )
	{
		std::stringstream ss( string );
		std::string subString;
		subString.reserve( string.length( ) );

		while ( std::getline( ss, subString, token ) )
		{
			params.emplace_back( std::move( subString ) );
		}
	}

	inline void SplitByBracket( const std::string& string, std::vector<std::string>& params, const char startToken, const char endToken )
	{
		size_t curIdx = 0;
		size_t startPos = string.find( startToken, curIdx );
		size_t endPos = string.find( endToken, ++startPos );

		while ( startPos != std::string::npos && endPos != std::string::npos )
		{
			params.push_back( string.substr( startPos, endPos - startPos ) );
			curIdx = ++endPos;

			startPos = string.find( startToken, curIdx );
			if ( startPos != std::string::npos )
			{
				endPos = string.find( endToken, ++startPos );
			}
		}
	}

	inline const std::string FileNameExtension( const std::string& pFileName )
	{
		auto found = pFileName.find_last_of( "." );

		return pFileName.substr( found + 1 );
	}

	inline std::string GetFileName( const char* pFilePath )
	{
		const char* start = strrchr( pFilePath, '/' );
		const char* end = strrchr( pFilePath, '.' );

		return std::string( start + 1, end );
	}

	inline std::string GetFilePath( const char* pFilePath )
	{
		if ( const char* end = strrchr( pFilePath, '.' ) )
		{
			const char* start = strrchr( pFilePath, '/' );
			return std::string( pFilePath, start + 1 );
		}
		
		return pFilePath;
	}

	inline void DebugMsgImplment( const char* msg, ... )
	{
		SetColor_White;

		char buf[1024] = { 0, };
		va_list vaList;

		va_start( vaList, msg );
		sprintf_s( buf, _countof( buf ), msg, vaList );
		va_end( vaList );

		printf_s( "%s", buf );
	}

	inline void DebugWarningImplment( const char* msg, ... )
	{
		SetColor_Red;

		char buf[1024] = { 0, };
		va_list vaList;

		va_start( vaList, msg );
		sprintf_s( buf, _countof( buf ), msg, vaList );
		va_end( vaList );

		printf_s( "%s", buf );
	}

	inline void ReplaceChar( char* dest, const char* src, char find, char replace, std::size_t destSize, std::size_t srcSize )
	{
		for ( uint32 i = 0; i < destSize && i < srcSize; ++i )
		{
			dest[i] = ( src[i] == find ) ? replace : src[i];
		}
	}
}

#if defined( _DEBUG ) || defined( DEBUGGING_BY_CONSOLE )
#define DebugMsg( x, ... ) UTIL::DebugMsgImplment( x, ##__VA_ARGS__ )
#define DebugWarning( x, ... ) UTIL::DebugWarningImplment( x, ##__VA_ARGS__ )
#else
#define DebugMsg( x, ... ) __noop
#define DebugWarning( x, ... ) __noop
#endif

#define _STR(x) #x
#define STR(x) _STR(x)
#define FIX_ME(x) __pragma(message("FIX ME: "_STR(x) " :: " __FILE__ "@"STR(__LINE__)))
