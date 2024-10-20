#pragma once

#include <cassert>
#include <fstream>
#include <string>
#include <tchar.h>
#include <Windows.h>

#ifdef CORE_EXPORTS
#define CORE_DLL __declspec(dllexport)
#define CORE_FUNC_DLL extern "C" __declspec(dllexport)
#else
#define CORE_DLL __declspec(dllimport)
#define CORE_FUNC_DLL extern "C" __declspec(dllimport)
#endif

#ifdef EDITOR_EXPORTS
#define EDITOR_DLL __declspec(dllexport)
#define EDITOR_FUNC_DLL extern "C" __declspec(dllexport)
#else
#define EDITOR_DLL __declspec(dllimport)
#define EDITOR_FUNC_DLL extern "C" __declspec(dllimport)
#endif

#ifdef ENGINE_EXPORTS
#define ENGINE_DLL __declspec(dllexport)
#define ENGINE_FUNC_DLL extern "C" __declspec(dllexport)
#else
#define ENGINE_DLL __declspec(dllimport)
#define ENGINE_FUNC_DLL extern "C" __declspec(dllimport)
#endif

#ifdef RENDERCORE_EXPORTS
#define RENDERCORE_DLL __declspec(dllexport)
#define RENDERCORE_FUNC_DLL extern "C" __declspec(dllexport)
#else
#define RENDERCORE_DLL __declspec(dllimport)
#define RENDERCORE_FUNC_DLL extern "C" __declspec(dllimport)
#endif

#ifdef LOGIC_EXPORTS
#define LOGIC_DLL __declspec(dllexport)
#define LOGIC_FUNC_DLL extern "C" __declspec(dllexport)
#else
#define LOGIC_DLL __declspec(dllimport)
#define LOGIC_FUNC_DLL extern "C" __declspec(dllimport)
#endif

#ifdef AGL_EXPORTS
#define AGL_DLL __declspec(dllexport)
#define AGL_FUNC_DLL extern "C" __declspec(dllexport)
#else
#define AGL_DLL __declspec(dllimport)
#define AGL_FUNC_DLL extern "C" __declspec(dllimport)
#endif

inline HMODULE LoadModule( const char* dllPath )
{
	HMODULE hModule = LoadLibraryA( dllPath );
	if ( hModule == nullptr )
	{
		assert( "Module loading failed!" && false );
	}

	using BootUpFunc = void(*)( );
	BootUpFunc bootUp = (BootUpFunc)( GetProcAddress( hModule, "BootUpModules" ) );
	if ( bootUp == nullptr )
	{
		assert( "Module must have BootUpModules function!" && false );
		return NULL;
	}

	bootUp( );

	return hModule;
}

inline void ShutdownModule( HMODULE dll )
{
	using ShutDownFunc = void(*)( );
	ShutDownFunc shutDown = reinterpret_cast<ShutDownFunc>( GetProcAddress( dll, "ShutdownModules" ) );
	if ( shutDown == nullptr )
	{
		assert( "Module must have ShutDownModules function!" && false );
		return;
	}

	shutDown( );

	FreeLibrary( dll );
}
