#pragma once

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

#define SAFE_RELEASE( x ) { \
if ( x ) \
	x->Release ( ); \
	x = NULL; \
}

#define ON_FAIL_RETURN( x ){ \
if ( !x ) \
	return false; \
}

#define ON_SUCCESS_RETURE( x ){ \
if ( x ) \
	return true; \
}
