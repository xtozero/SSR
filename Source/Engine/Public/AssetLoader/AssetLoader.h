#pragma once

#include "Delegate.h"
#include "GuideTypes.h"
#include "SizedTypes.h"

#include <memory>
#include <string>

class AssetLoaderHandle;

using AssetLoaderSharedHandle = std::shared_ptr<AssetLoaderHandle>;

class IAssetLoader
{
public:
	using LoadCompletionCallback = Delegate<void, const std::shared_ptr<void>&>;

	virtual AssetLoaderSharedHandle RequestAsyncLoad( const std::string& assetPath, LoadCompletionCallback completionCallback ) = 0;
	virtual AssetLoaderSharedHandle HandleInProcess( ) const = 0;
	virtual void SetHandleInProcess( const AssetLoaderSharedHandle& handle ) = 0;

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

	void ExecuteCompletionCallback( );

	void SetLoadedAsset( const std::shared_ptr<void>& loadedAsset )
	{
		m_loadingInProgress = false;
		m_loadComplete = ( loadedAsset != nullptr );
		assert( m_loadComplete );

		m_loadedAsset = loadedAsset;
	}

	void BindCompletionCallback( const IAssetLoader::LoadCompletionCallback& callback )
	{
		m_loadCompletionCallback = callback;
	}

	void AddPrerequisite( const AssetLoaderSharedHandle& prerequisite )
	{
		prerequisite->m_subSequentList.emplace_back( shared_from_this() );
		IncreasePrerequisite( );
	}

	void OnStartLoading( )
	{
		m_loadingInProgress = true;
	}

	void NeedPostProcess( )
	{
		m_needPostProcess = true;
	}

	bool HasPrerequisites( ) const
	{
		return m_prerequisites > 0;
	}

	void IncreasePrerequisite( )
	{
		++m_prerequisites;
	}

	void DecreasePrerequisite( )
	{
		--m_prerequisites;
	}

private:
	std::vector<AssetLoaderSharedHandle> m_subSequentList;
	int32 m_prerequisites = 0;

	IAssetLoader::LoadCompletionCallback m_loadCompletionCallback;
	std::shared_ptr<void> m_loadedAsset = nullptr;
	bool m_loadingInProgress = false;
	bool m_loadComplete = false;
	bool m_needPostProcess = false;
};
