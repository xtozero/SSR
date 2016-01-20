#pragma once

#include <string>
#include <d3d11.h>

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
typedef std::string String;
#else
typedef std::wstring String;
#endif

#define ON_FAIL_RETURN( x ){ \
if ( !x ) \
	return false; \
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

#define FOR_EACH_VEC( x, i ) \
for ( auto i = x.begin( ); i != x.end( ); ++i )

#define FOR_EACH_MAP( x, i ) \
for ( auto i = x.begin( ); i != x.end( ); ++i )