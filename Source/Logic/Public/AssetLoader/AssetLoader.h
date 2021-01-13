#pragma once

#include "Delegate.h"
#include "Util.h"

#include <memory>
#include <string>

class AssetLoaderHandle;

using AssetLoaderSharedHandle = std::shared_ptr<AssetLoaderHandle>;

class IAssetLoader
{
public:
	using LoadCompletionCallback = Delegate<void, const std::shared_ptr<void>&>;

	virtual AssetLoaderSharedHandle RequestAsyncLoad( const std::string& assetPath, LoadCompletionCallback completionCallback ) = 0;

	virtual ~IAssetLoader( ) = default;
};

Owner<IAssetLoader*> CreateAssetLoader( );
void DestoryAssetLoader( Owner<IAssetLoader*> pRenderOptionManager );

class AssetLoaderHandle : public std::enable_shared_from_this<AssetLoaderHandle>
{
public:
	bool IsLoadingInProgress( ) const
	{
		return m_loadingInProgress;
	}

	bool IsLoadComplete( ) const
	{
		return m_loadComplete;
	}

	void ExecuteCompletionCallback( )
	{
		if ( m_loadComplete == false )
		{
			return;
		}

		if ( m_prerequisites > 0 )
		{
			return;
		}
		
		if ( m_loadCompletionCallback.IsBound( ) )
		{
			m_loadCompletionCallback( m_loadedAsset );
		}

		for ( const auto& subSequent : m_subSequentList )
		{
			subSequent->OnAchievePrerequisite( );
			subSequent->ExecuteCompletionCallback( );
		}
	}

	void SetLoadedAsset( const std::shared_ptr<void>& loadedAsset )
	{
		m_loadingInProgress = false;
		m_loadComplete = ( loadedAsset != nullptr );

		m_loadedAsset = loadedAsset;
	}

	void BindCompletionCallback( const IAssetLoader::LoadCompletionCallback& callback )
	{
		m_loadCompletionCallback = callback;
	}

	void AddPrerequisite( const AssetLoaderSharedHandle& prerequisite )
	{
		prerequisite->m_subSequentList.emplace_back( shared_from_this() );
		++m_prerequisites;
	}

	void OnStartLoading( )
	{
		m_loadingInProgress = true;
	}

private:
	void OnAchievePrerequisite( )
	{
		--m_prerequisites;
	}

	std::vector<AssetLoaderSharedHandle> m_subSequentList;
	int m_prerequisites = 0;

	IAssetLoader::LoadCompletionCallback m_loadCompletionCallback;
	std::shared_ptr<void> m_loadedAsset = nullptr;
	bool m_loadingInProgress = false;
	bool m_loadComplete = false;
};
