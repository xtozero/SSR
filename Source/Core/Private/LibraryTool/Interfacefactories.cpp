#include "InterfaceFactories.h"

#include <cassert>

InterfaceFactories g_InterfaceFactories;

void InterfaceFactories::RegisterFactory( std::type_index typeIndex, FactoryFunctionType factoryFunc )
{
	auto found = m_factoryFuncs.find( typeIndex );
	if ( found != m_factoryFuncs.end( ) )
	{
		assert( "Duplicate factory function" && false );
	}

	m_factoryFuncs.emplace( typeIndex, factoryFunc );
}

void InterfaceFactories::UnregisterFactory( std::type_index typeIndex )
{
	m_factoryFuncs.erase( typeIndex );
}

FactoryFunctionType InterfaceFactories::GetFactoryFunction( std::type_index typeIndex ) const
{
	auto found = m_factoryFuncs.find( typeIndex );
	if ( found != m_factoryFuncs.end( ) )
	{
		return found->second;
	}

	return nullptr;
}

InterfaceFactories& InterfaceFactories::Get( )
{
	return g_InterfaceFactories;
}

void* GetInterface( std::type_index typeIndex )
{
	FactoryFunctionType factoryFunc = InterfaceFactories::Get( ).GetFactoryFunction( typeIndex );
	if ( factoryFunc )
	{
		return factoryFunc( );
	}

	return nullptr;
}

void RegisterFactory( std::type_index typeIndex, FactoryFunctionType factoryFunc )
{
	InterfaceFactories::Get( ).RegisterFactory( typeIndex, factoryFunc );
}

void UnregisterFactory( std::type_index typeIndex )
{
	InterfaceFactories::Get( ).UnregisterFactory( typeIndex );
}