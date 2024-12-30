#include "AssetFactory.h"

class AssetFactory : public IAssetFactory
{
public:
	IAsyncLoadableAsset* CreateAsset( uint32 assetId ) const override
	{
		auto found = m_createfunctions.find( assetId );
		if ( found != m_createfunctions.end( ) )
		{
			return found->second( );
		}

		return nullptr;
	}

	virtual void AddCreateFunction( uint32 assetId, AssetCreateFunctionPtr func ) override
	{
		assert( m_createfunctions.contains( assetId ) == false );
		m_createfunctions.emplace( assetId, func );
	}

private:
	std::map<uint32, AssetCreateFunctionPtr> m_createfunctions;
};

Owner<IAssetFactory*> CreateAssetFactory( )
{
	return new AssetFactory( );
}

void DestroyAssetFactory( Owner<IAssetFactory*> pAssetFactory )
{
	delete pAssetFactory;
}
