#pragma once

#include "IAsyncLoadableAsset.h"
#include "Delegate.h"

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
		T::ID = std::hash<std::string>{ }( assetType );

		Delegate<IAsyncLoadableAsset*> func;
		func.BindFunction( &NewAsset<T> );

		assert( m_createfunctions.find( T::ID ) == std::end( m_createfunctions ) );

		m_createfunctions.emplace( T::ID, std::move( func ) );
	}

	ENGINE_DLL IAsyncLoadableAsset* CreateAsset( std::size_t assetID );

private:
	AssetFactory( ) = default;

	std::map<std::size_t, Delegate<IAsyncLoadableAsset*>> m_createfunctions;
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
	dllName##_DLL static std::size_t ID

#define REGISTER_ASSET( type ) \
	std::size_t type::ID = 0; \
	const AssetFactoryRegister<type> type##Register( #type );