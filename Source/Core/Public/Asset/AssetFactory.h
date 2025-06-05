#pragma once

#include "CrcHash.h"
#include "IAsyncLoadableAsset.h"
#include "InterfaceFactories.h"
#include "SizedTypes.h"

#include <functional>
#include <map>
#include <typeindex>
#include <vector>

using AssetCreateFunctionPtr = IAsyncLoadableAsset* (*)();

class IAssetFactory
{
public:
	void RegisterCreateFunction( uint32 id, AssetCreateFunctionPtr createFunc )
	{
		AddCreateFunction( id, createFunc );
	}

	virtual IAsyncLoadableAsset* CreateAsset( uint32 assetId ) const = 0;

	virtual ~IAssetFactory() = default;

protected:
	virtual void AddCreateFunction( uint32 assetId, AssetCreateFunctionPtr func ) = 0;
};

template <typename T>
IAsyncLoadableAsset* NewAsset()
{
	return new T();
}

class DeferredAssetRegister
{
public:
	static DeferredAssetRegister& GetInstance()
	{
		static DeferredAssetRegister deferredAssetRegister;
		return deferredAssetRegister;
	}

	void Register() const
	{
		auto* assetFactory = GetInterface<IAssetFactory>();
		for ( const auto& pair : m_functionPairs )
		{
			auto& [id, createFunction] = pair;
			assetFactory->RegisterCreateFunction( id, createFunction );
		}
	}

	template <typename T>
	void AddCreateFunction( const char* assetType )
	{
		T::Id = Crc32Hash( assetType );
		m_functionPairs.emplace_back( T::Id, &NewAsset<T> );
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
		DeferredAssetRegister::GetInstance().AddCreateFunction<T>( assetType );
	}
};

Owner<IAssetFactory*> CreateAssetFactory();
void DestroyAssetFactory( Owner<IAssetFactory*> pAssetFactory );

#define DECLARE_ASSET_WITH_VERSION( dllName, type, version ) \
public : \
	virtual uint32 GetId() const override \
	{ \
		return type::Id; \
	} \
\
	virtual uint32 GetAssetVersion() const override \
	{ \
		return version; \
	} \
\
	dllName##_DLL static uint32 Id; \
private : \
\

#define DECLARE_ASSET( dllName, type ) DECLARE_ASSET_WITH_VERSION( dllName, type, 0 )

#define REGISTER_ASSET( type ) \
	uint32 type::Id = 0; \
	const AssetFactoryRegister<type> type##Register( #type );