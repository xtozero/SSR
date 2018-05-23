#pragma once

#include <cassert>
#include <sstream>
#include <tchar.h>
#include <vector>

#ifndef UNICODE
using String = std::string;
using Ifstream = std::ifstream;
using Stringstream = std::stringstream;
#define Cout std::cout
#else
using String = std::wstring;
using Ifstream = std::wifstream;
using Stringstream = std::wstringstream;
#define Cout std::wcout
#endif

#define col GetStdHandle( STD_OUTPUT_HANDLE )
#define SetColor_Red SetConsoleTextAttribute( col, 0x000c )
#define SetColor_Blue SetConsoleTextAttribute( col, 0x0001 | 0x0008 )
#define SetColor_Green SetConsoleTextAttribute( col, 0x0001 | 0x0008 )
#define SetColor_White SetConsoleTextAttribute( col, 0x000f )
#define SetColor_Skyblue SetConsoleTextAttribute( col, 0x000b )
#define SetColor_Yellow SetConsoleTextAttribute( col, 0x000e )
#define SetColor_Chemical SetConsoleTextAttribute( col, 0x000d )
#define SetColor_Gold SetConsoleTextAttribute( col, 0x0006 )
#define SetColor_Wine SetConsoleTextAttribute( col, 0x0005 )
#define SetColor_Blood SetConsoleTextAttribute( col, 0x0004 )
#define SetColor_Violet SetConsoleTextAttribute( col, 0x0001 | 0x0008 | 0x000c )

namespace UTIL
{
	inline void Split( const String& string, std::vector<String>& params, const TCHAR token )
	{
		Stringstream ss( string );
		String subString;
		subString.reserve( string.length( ) );

		while ( std::getline( ss, subString, token ) )
		{
			params.emplace_back( std::move( subString ) );
		}
	}

	inline void SplitByBracket( const String& string, std::vector<String>& params, const TCHAR startToken, const TCHAR endToken )
	{
		auto curIdx = 0;
		auto startPos = string.find( startToken, curIdx );
		auto endPos = string.find( endToken, ++startPos );

		while ( startPos != String::npos && endPos != String::npos )
		{
			params.push_back( string.substr( startPos, endPos - startPos ) );
			curIdx = ++endPos;

			startPos = string.find( startToken, curIdx );
			if ( startPos != String::npos )
			{
				endPos = string.find( endToken, ++startPos );
			}
		}
	}

	inline const String FileNameExtension( const String& pFileName )
	{
		auto found = pFileName.find_last_of( _T( "." ) );

		return pFileName.substr( found + 1 );
	}

	inline String GetFileName( const TCHAR* pFilePath )
	{
		const TCHAR* start = _tcsrchr( pFilePath, _T( '/' ) );
		const TCHAR* end = _tcsrchr( pFilePath, _T( '.' ) );

		return String( start + 1, end );
	}

	inline void DebugMsgImplment( const TCHAR* msg, ... )
	{
		SetColor_White;

		TCHAR buf[1024] = { 0, };
		va_list vaList;

		va_start( vaList, msg );
		_vstprintf_s( buf, _countof( buf ), msg, vaList );
		va_end( vaList );

		_tprintf_s( _T( "%s" ), buf );
	}

	inline void DebugWarningImplment( const TCHAR* msg, ... )
	{
		SetColor_Red;

		TCHAR buf[1024] = { 0, };
		va_list vaList;

		va_start( vaList, msg );
		_vstprintf_s( buf, _countof( buf ), msg, vaList );
		va_end( vaList );

		_tprintf_s( _T( "%s" ), buf );
	}

	inline void KeyValueAssert( String value, UINT count )
	{
		std::vector<String> params;
		UTIL::Split( value, params, _T( ' ' ) );
		assert( params.size( ) == count );
	}
}

#if defined( _DEBUG ) || defined( DEBUGGING_BY_CONSOLE )
#define DebugMsg( x, ... ) UTIL::DebugMsgImplment( _T( x ), ##__VA_ARGS__ )
#define DebugWarning( x, ... ) UTIL::DebugWarningImplment( _T( x ), ##__VA_ARGS__ )
#else
#define DebugMsg( x, ... ) __noop
#define DebugWarning( x, ... ) __noop
#endif

#define FOR_EACH_VEC( x, i ) \
for ( auto i = x.begin( ); i != x.end( ); ++i )

#define FOR_EACH_MAP( x, i ) \
for ( auto i = x.begin( ); i != x.end( ); ++i )

#ifdef _DEBUG
#define KEYVALUE_VALUE_ASSERT( value, count ) UTIL::KeyValueAssert( value, count )
#else
#define KEYVALUE_VALUE_ASSERT( value, count ) __noop
#endif

#define _STR(x) #x
#define STR(x) _STR(x)
#define FIX_ME(x) __pragma(message("FIX ME: "_STR(x) " :: " __FILE__ "@"STR(__LINE__)))

template <typename T>
using Owner = T;