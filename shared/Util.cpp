#include "stdafx.h"

#include "Util.h"
#include <tchar.h>

namespace UTIL
{
	void Split( const String& string, std::vector<String>& params, const TCHAR token )
	{
		Stringstream ss( string );
		String subString;
		subString.reserve( string.length( ) );

		while ( std::getline( ss, subString, token ) )
		{
			params.emplace_back( std::move( subString ) );
		}
	}

	void SplitByBracket( const String& string, std::vector<String>& params, const TCHAR startToken, const TCHAR endToken )
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

	const String FileNameExtension( const String& pFileName )
	{
		auto found = pFileName.find_last_of( _T( "." ) );

		return pFileName.substr( found + 1 );
	}

	String GetFileName( const TCHAR* pFilePath )
	{
		const TCHAR* start = _tcsrchr( pFilePath, _T( '/' ) );
		const TCHAR* end = _tcsrchr( pFilePath, _T( '.' ) );

		return String( start + 1, end );
	}

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