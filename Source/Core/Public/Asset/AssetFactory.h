#pragma once

#include "Crc32Hash.h"
#include "Delegate.h"
#include "IAsyncLoadableAsset.h"
#include "InterfaceFactories.h"
#include "SizedTypes.h"

#include <functional>
#include <map>
#include <typeindex>

class IAssetFactory
{
public:
	template <typename T>
	void RegisterAsset( const char* assetType )
	{
		T::ID = Crc32Hash( assetType );

		Delegate<IAsyncLoadableAsset*> func;
		func.BindFunction( &NewAsset<T> );

		AddCreateFunction( T::ID, std::move( func ) );
	}

	virtual IAsyncLoadableAsset* CreateAsset( uint32 assetID ) const = 0;

	virtual ~IAssetFactory( ) = default;

protected:
	virtual void AddCreateFunction( uint32 assetID, Delegate<IAsyncLoadableAsset*>&& func ) = 0;
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
		GetInterface<IAssetFactory>( )->RegisterAsset<T>( assetType );
	}
};

Owner<IAssetFactory*> CreateAssetFactory( );
void DestoryAssetFactory( Owner<IAssetFactory*> pAssetFactory );

#define DECLARE_ASSET( dllName, type ) \
public : \
	dllName##_DLL static uint32 ID

#define REGISTER_ASSET( type ) \
	uint32 type::ID = 0; \
	const AssetFactoryRegister<type> type##Register( #type );