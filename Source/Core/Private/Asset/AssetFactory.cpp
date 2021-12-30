#include "AssetFactory.h"

class AssetFactory : public IAssetFactory
{
public:
	IAsyncLoadableAsset* CreateAsset( uint32 assetID ) const override
	{
		auto found = m_createfunctions.find( assetID );
		if ( found != m_createfunctions.end( ) )
		{
			return found->second( );
		}

		return nullptr;
	}

	virtual void AddCreateFunction( uint32 assetID, Delegate<IAsyncLoadableAsset*>&& func ) override
	{
		assert( m_createfunctions.find( assetID ) == std::end( m_createfunctions ) );
		m_createfunctions.emplace( assetID, std::move( func ) );
	}

private:
	std::map<uint32, Delegate<IAsyncLoadableAsset*>> m_createfunctions;
};

Owner<IAssetFactory*> CreateAssetFactory( )
{
	return new AssetFactory( );
}

void DestoryAssetFactory( Owner<IAssetFactory*> pAssetFactory )
{
	delete pAssetFactory;
}