#include "AssetLoader\AssetFactory.h"

AssetFactory& AssetFactory::GetInstance( )
{
	static AssetFactory factory;
	return factory;
}

void AssetFactory::Shutdown( )
{
	m_createfunctions.clear( );
}

IAsyncLoadableAsset* AssetFactory::CreateAsset( std::size_t assetID )
{
	auto found = m_createfunctions.find( assetID );
	if ( found != m_createfunctions.end( ) )
	{
		return found->second( );
	}

	return nullptr;
}
