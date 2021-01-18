#pragma once

#include "IAsyncLoadableAsset.h"
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
	void RegisterAsset( )
	{
		if ( T::ID == -1 )
		{
			T::ID = m_lastAssetID++;

			Delegate<IAsyncLoadableAsset*> func;
			func.BindFunction( &NewAsset<T> );

			m_createfunctions.emplace( T::ID, std::move( func ) );
		}
	}

	IAsyncLoadableAsset* CreateAsset( int assetID )
	{
		auto found = m_createfunctions.find( assetID );
		if ( found != m_createfunctions.end( ) )
		{
			return found->second( );
		}

		return nullptr;
	}

private:
	std::map<int, Delegate<IAsyncLoadableAsset*>> m_createfunctions;
	int m_lastAssetID = 0;
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
	AssetFactoryRegister( )
	{
		AssetFactory::GetInstance( ).RegisterAsset<T>( );
	}
};

#define DECLARE_ASSET( type ) \
public : \
	LOGIC_DLL static int ID

#define REGISTER_ASSET( type ) \
	int type::ID = -1; \
	const AssetFactoryRegister<type> type##Register;