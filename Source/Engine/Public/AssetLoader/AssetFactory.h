#pragma once

#include "Crc32Hash.h"
#include "Delegate.h"
#include "IAsyncLoadableAsset.h"
#include "SizedTypes.h"

#include <functional>
#include <map>
#include <typeindex>

class AssetFactory
{
public:
	ENGINE_DLL static AssetFactory& GetInstance( );

	void Shutdown( );

	template <typename T>
	void RegisterAsset( const char* assetType )
	{
		T::ID = Crc32Hash( assetType );

		Delegate<IAsyncLoadableAsset*> func;
		func.BindFunction( &NewAsset<T> );

		assert( m_createfunctions.find( T::ID ) == std::end( m_createfunctions ) );

		m_createfunctions.emplace( T::ID, std::move( func ) );
	}

	ENGINE_DLL IAsyncLoadableAsset* CreateAsset( uint32 assetID );

private:
	AssetFactory( ) = default;

	std::map<uint32, Delegate<IAsyncLoadableAsset*>> m_createfunctions;
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
	AssetFactoryRegister( const char* assetType )
	{
		AssetFactory::GetInstance( ).RegisterAsset<T>( assetType );
	}
};

#define DECLARE_ASSET( dllName, type ) \
public : \
	dllName##_DLL static uint32 ID

#define REGISTER_ASSET( type ) \
	uint32 type::ID = 0; \
	const AssetFactoryRegister<type> type##Register( #type );