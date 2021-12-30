#pragma once

#include "Crc32Hash.h"
#include "Delegate.h"
#include "IAsyncLoadableAsset.h"
#include "InterfaceFactories.h"
#include "SizedTypes.h"

#include <functional>
#include <map>
#include <typeindex>
#include <vector>

using AssetCreateFunctionPtr = IAsyncLoadableAsset * (*)( );

class IAssetFactory
{
public:
	void RegisterCreateFunction( uint32 id, AssetCreateFunctionPtr createFunc )
	{
		Delegate<IAsyncLoadableAsset*> func;
		func.BindFunction( createFunc );

		AddCreateFunction( id, std::move( func ) );
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

class DeferredAssetRegister
{
public:
	static DeferredAssetRegister& GetInstance( )
	{
		static DeferredAssetRegister deferredAssetRegister;
		return deferredAssetRegister;
	}

	void Register( ) const
	{
		auto* assetFactory = GetInterface<IAssetFactory>( );
		for ( const auto& pair : m_functionPairs )
		{
			auto& [id, createFunction] = pair;
			assetFactory->RegisterCreateFunction( id, createFunction );
		}
	}

	template <typename T>
	void AddCreateFunction( const char* assetType )
	{
		T::ID = Crc32Hash( assetType );
		m_functionPairs.emplace_back( T::ID, &NewAsset<T> );
	}

private:
	using AssetCreateFunctionPair = std::pair<uint32, AssetCreateFunctionPtr>;
	std::vector<AssetCreateFunctionPair> m_functionPairs;
};

template <typename T>
class AssetFactoryRegister
{
public:
	AssetFactoryRegister( const char* assetType )
	{
		DeferredAssetRegister::GetInstance( ).AddCreateFunction<T>( assetType );
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