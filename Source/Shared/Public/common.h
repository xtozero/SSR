#pragma once

#include <cassert>
#include <fstream>
#include <string>
#include <tchar.h>

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

#ifndef UNICODE
using String = std::string;
using Ifstream = std::ifstream;
#define TO_STRING( x ) std::to_string( x )
#else
using String = std::wstring;
using Ifstream = std::wifstream;
#define TO_STRING( x ) std::to_wstring( x )
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