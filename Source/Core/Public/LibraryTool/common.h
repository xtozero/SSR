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

#ifdef AGA_EXPORTS
#define AGA_DLL __declspec(dllexport)
#define AGA_FUNC_DLL extern "C" __declspec(dllexport)
#else
#define AGA_DLL __declspec(dllimport)
#define AGA_FUNC_DLL extern "C" __declspec(dllimport)
#endif

#define ON_FAIL_RETURN( x ){ \
	if ( !x ) \
	{\
		assert( false );\
		return false; \
	}\
}

struct InPlaceListNode
{
	unsigned char* m_next;
};

template <typename T>
void PushFrontInPlaceList( unsigned char** head, T* newElement )
{
	static_assert( sizeof( T ) >= sizeof( InPlaceListNode ), "Free-List element memory size must bigger than FreeResourceNode" );
	InPlaceListNode* newFront = reinterpret_cast<InPlaceListNode*>( newElement );
	newFront->m_next = *head;
	*head = reinterpret_cast<unsigned char*>( newElement );
}

template <typename T>
void PopFrontInPlaceList( unsigned char** head, T** dest )
{
	static_assert( sizeof( T ) >= sizeof( InPlaceListNode ), "Free-List element memory size must bigger than FreeResourceNode" );
	*dest = reinterpret_cast<T*>( *head );
	InPlaceListNode* node = reinterpret_cast<InPlaceListNode*>( *head );
	*head = node->m_next;
	*dest = new ( *dest )T;
}

template <typename T>
void ClearFreeList( unsigned char** head )
{
	static_assert( sizeof( T ) >= sizeof( InPlaceListNode ), "Free-List element memory size must bigger than FreeResourceNode" );
	while ( *head != nullptr )
	{
		T* elem = reinterpret_cast<T*>( *head );
		InPlaceListNode* node = reinterpret_cast<InPlaceListNode*>( *head );
		*head = node->m_next;
		elem = new ( elem )T;
	}
}

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
