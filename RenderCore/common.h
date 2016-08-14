#pragma once

#include <string>
#include <cassert>

#ifdef RENDERCORE_EXPORTS
#define RENDERCORE_DLL __declspec(dllexport)
#else
#define RENDERCORE_DLL __declspec(dllimport)
#endif

#ifdef RENDERCORE_EXPORTS
#define RENDERCORE_FUNC_DLL extern "C" __declspec(dllexport)
#else
#define RENDERCORE_FUNC_DLL extern "C" __declspec(dllimport)
#endif

#ifndef UNICODE  
using String = std::string;
#else
using String = std::wstring;
#endif

#define ON_FAIL_RETURN( x ){ \
	if ( !x ) \
	{\
		assert( false );\
		return false; \
	}\
}

#define ON_SUCCESS_RETURE( x ){ \
if ( x ) \
	return true; \
}

#define SAFE_DELETE( x ) { \
if ( x ) \
	delete x; \
	x = nullptr; \
}

#define SAFE_ARRAY_DELETE( x ) { \
if ( x ) \
	delete []x; \
	x = nullptr; \
}