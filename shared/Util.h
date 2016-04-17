#pragma once

#include <tchar.h>
#include <vector>
#include <sstream>
#include <cassert>

#ifndef UNICODE  
typedef std::string String;
typedef std::ifstream Ifstream;
typedef std::stringstream Stringstream;
#define Cout std::cout
#else
typedef std::wstring String;
typedef std::wifstream Ifstream;
typedef std::wstringstream Stringstream;
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
	void Split( const String& string, std::vector<String>& params, const TCHAR token );
	void SplitByBracket( const String& string, std::vector<String>& params, const TCHAR startToken, const TCHAR endToken );
	const String FileNameExtension( const String& pFileName );
	String GetFileName( const TCHAR* pFilePath );
}

namespace
{
	void DebugMsgImplment( const TCHAR* msg, ... )
	{
		SetColor_White;

		TCHAR buf[1024] = { 0, };
		va_list vaList;

		va_start( vaList, msg );
		_vstprintf_s( buf, _countof( buf ), msg, vaList );
		va_end( vaList );

		_tprintf_s( _T( "%s" ), buf );
	}

	void DebugWarningImplment( const TCHAR* msg, ... )
	{
		SetColor_Red;

		TCHAR buf[1024] = { 0, };
		va_list vaList;

		va_start( vaList, msg );
		_vstprintf_s( buf, _countof( buf ), msg, vaList );
		va_end( vaList );

		_tprintf_s( _T( "%s" ), buf );
	}

	void KeyValueAssert( String value, UINT count )
	{
		std::vector<String> params;
		UTIL::Split( value, params, _T( ' ' ) );
		assert( params.size( ) == count );
	}
}

#ifdef _DEBUG
#define DebugMsg( x, ... ) DebugMsgImplment( _T( x ), ##__VA_ARGS__ )
#define DebugWarning( x, ... ) DebugWarningImplment( _T( x ), ##__VA_ARGS__ )
#else
#define DebugMsg( x, ... ) __noop
#define DebugWarning( x, ... ) __noop
#endif

#define FOR_EACH_VEC( x, i ) \
for ( auto i = x.begin( ); i != x.end( ); ++i )

#define FOR_EACH_MAP( x, i ) \
for ( auto i = x.begin( ); i != x.end( ); ++i )

#ifdef _DEBUG
#define KEYVALUE_VALUE_ASSERT( value, count ) KeyValueAssert( value, count )
#else
#define KEYVALUE_VALUE_ASSERT( value, count ) __noop
#endif
