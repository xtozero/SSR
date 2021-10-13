#pragma once

#include "common.h"

#include <map>
#include <typeindex>

using FactoryFunctionType = void*(*)( );

class InterfaceFactories
{
public:
	void RegisterFactory( std::type_index typeIndex, FactoryFunctionType factoryFunc );
	void UnregisterFactory( std::type_index typeIndex );
	FactoryFunctionType GetFactoryFunction( std::type_index typeIndex ) const;
	static InterfaceFactories& Get( );

private:
	std::map<std::type_index, FactoryFunctionType> m_factoryFuncs;
};

#ifdef ENGINE_EXPORTS
template <typename T>
T* GetInterface( )
{
	FactoryFunctionType factoryFunc = InterfaceFactories::Get( ).GetFactoryFunction( typeid( T ) );
	if ( factoryFunc )
	{
		return static_cast<T*>( factoryFunc( ) );
	}

	return nullptr;
}

template <typename T>
void RegisterFactory( FactoryFunctionType factoryFunc )
{
	InterfaceFactories::Get( ).RegisterFactory( typeid( T ), factoryFunc );
}

template <typename T>
void UnregisterFactory( )
{
	InterfaceFactories::Get( ).UnregisterFactory( typeid( T ) );
}
#else
template <typename T>
T* GetInterface( )
{
	HMODULE hEngineModule = GetModuleHandleA( "Engine.dll" );
	if ( hEngineModule == nullptr )
	{
		assert( "Engine module not loaded!" && false );
	}

	using GetInterfaceFunc = void*(*)( std::type_index );
	GetInterfaceFunc getInterface = reinterpret_cast<GetInterfaceFunc>( GetProcAddress( hEngineModule, "GetInterface" ) );
	if ( getInterface == nullptr )
	{
		assert( "Engine module must have GetInterface function!" && false );
	}

	return static_cast<T*>( getInterface( typeid( T ) ) );
}

template <typename T>
void RegisterFactory( FactoryFunctionType factoryFunc )
{
	HMODULE hEngineModule = GetModuleHandleA( "Engine.dll" );
	if ( hEngineModule == nullptr )
	{
		assert( "Engine module not loaded!" && false );
	}

	using RegisterFactoryFunc = void(*)( std::type_index, FactoryFunctionType );
	RegisterFactoryFunc registerFactory = reinterpret_cast<RegisterFactoryFunc>( GetProcAddress( hEngineModule, "RegisterFactory" ) );
	if ( registerFactory == nullptr )
	{
		assert( "Engine module must have RegisterFactory function!" && false );
	}

	registerFactory( typeid( T ), factoryFunc );
}

template <typename T>
void UnregisterFactory( )
{
	HMODULE hEngineModule = GetModuleHandleA( "Engine.dll" );
	if ( hEngineModule == nullptr )
	{
		assert( "Engine module not loaded!" && false );
	}

	using UnregisterFactoryFunc = void(*)( std::type_index );
	UnregisterFactoryFunc unregisterFactory = reinterpret_cast<UnregisterFactoryFunc>( GetProcAddress( hEngineModule, "UnregisterFactory" ) );
	if ( unregisterFactory == nullptr )
	{
		assert( "Engine module must have UnregisterFactory function!" && false );
	}

	unregisterFactory( typeid( T ) );
}
#endif

CORE_FUNC_DLL void* GetInterface( std::type_index typeIndex );
CORE_FUNC_DLL void RegisterFactory( std::type_index typeIndex, FactoryFunctionType factoryFunc );
CORE_FUNC_DLL void UnregisterFactory( std::type_index typeIndex );
