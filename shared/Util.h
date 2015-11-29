#pragma once

#include <tchar.h>
#include <vector>

#ifndef UNICODE  
typedef std::string String;
typedef std::ifstream Ifstream;
#define Cout std::cout
#else
typedef std::wstring String;
typedef std::wifstream Ifstream;
#define Cout std::wcout
#endif

namespace UTIL
{
	void Split( const String& string, std::vector<String>& params, const TCHAR token );
	void SplitByBracket( const String& string, std::vector<String>& params, const TCHAR startToken, const TCHAR endToken );
}