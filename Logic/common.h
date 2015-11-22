#pragma once

#include <cstdio>
#include <cstdarg>
#include <iostream>

#ifdef LOGIC_EXPORTS
#define LOGIC_DLL __declspec(dllexport)
#else
#define LOGIC_DLL __declspec(dllimport)
#endif

#ifndef UNICODE  
typedef std::string String;
#else
typedef std::wstring String;
#endif

namespace
{
	void DebugMsgImplment ( const char* msg, ... )
	{
		char buf[1024] = { 0, };
		va_list vaList;

		va_start ( vaList, msg );
		vsprintf_s ( buf, sizeof(buf), msg, vaList );
		va_end ( vaList );

		printf_s ( "%s", buf );
	}
}

#ifdef _DEBUG
#define DebugMsg DebugMsgImplment
#else
#define DebugMsg __noop
#endif