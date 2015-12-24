#include "stdafx.h"

#include "Util.h"
#include <tchar.h>

namespace UTIL
{
	void Split( const String& string, std::vector<String>& params, const TCHAR token )
	{
		auto i = 0;
		auto pos = string.find( token );
		while ( pos != String::npos ) {
			params.push_back( string.substr( i, pos - i ) );
			i = ++pos;
			pos = string.find( token, pos );

			if ( pos == String::npos )
			{
				params.push_back( string.substr( i, string.length( ) - i ) );
			}
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
}