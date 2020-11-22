#pragma once

#include "Core/IAsyncLoadableAsset.h"
#include "Delegate.h"

#include <map>
#include <typeindex>

class AssetFactory
{
public:
	static AssetFactory& GetInstance( )
	{
		static AssetFactory factory;
		return factory;
	}

	template <typename T>
	void RegisterAsset( const std::string& typeName )
	{
		Delegate<IAsyncLoadableAsset*> func;
		func.BindFunction( &NewAsset<T> );
		m_createfunctions.emplace( typeName, std::move( func ) );
	}

	IAsyncLoadableAsset* CreateAsset( const std::string& typeName )
	{
		auto found = m_createfunctions.find( typeName );
		if ( found != m_createfunctions.end( ) )
		{
			return found->second( );
		}

		return nullptr;
	}

private:
	std::map<std::string, Delegate<IAsyncLoadableAsset*>> m_createfunctions;
};

template <typename T>
IAsyncLoadableAsset* NewAsset( )
{
	return new T();
}

template <typename T>
class AssetFactoryRegister
{
public:
	AssetFactoryRegister( const std::string& typeName )
	{
		AssetFactory::GetInstance( ).RegisterAsset<T>( typeName );
	}
};

#define DECLARE_ASSET( type ) \
	static AssetFactoryRegister<type> type##Register( #type );

#define DECLARE_NAMED_ASSET( type, name ) \
	static AssetFactoryRegister<type> type##Register( #name );