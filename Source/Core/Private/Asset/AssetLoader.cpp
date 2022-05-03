#include "AssetLoader.h"

#include "Archive.h"
#include "AssetFactory.h"
#include "FileSystem.h"
#include "IAsyncLoadableAsset.h"
#include "InterfaceFactories.h"
#include "Json/json.hpp"
#include "SizedTypes.h"

#include <filesystem>
#include <memory>
#include <unordered_map>

void AssetLoaderHandle::ExecuteCompletionCallback()
{
	assert( IsInGameThread() );

	if ( m_prerequisites > 0 )
	{
		return;
	}

	if ( m_loadComplete == false )
	{
		return;
	}

	if ( m_needPostProcess )
	{
		if ( auto pAsyncLoadable = std::static_pointer_cast<AsyncLoadableAsset>( m_loadedAsset ) )
		{
			pAsyncLoadable->PostLoad();
		}
		m_needPostProcess = false;
	}

	if ( m_loadCompletionCallback.IsBound() )
	{
		m_loadCompletionCallback( m_loadedAsset );
		m_loadCompletionCallback.Unbind();
	}

	for ( const auto& subSequent : m_subSequentList )
	{
		subSequent->DecreasePrerequisite();
		subSequent->ExecuteCompletionCallback();
	}

	m_subSequentList.clear();
}

class AssetLoader : public IAssetLoader
{
public:
	virtual AssetLoaderSharedHandle RequestAsyncLoad( const std::string& assetPath, LoadCompletionCallback completionCallback ) override;
	virtual AssetLoaderSharedHandle HandleInProcess() const override;
	virtual void SetHandleInProcess( const AssetLoaderSharedHandle& handle ) override;

	AssetLoader() = default;
	~AssetLoader() = default;
	AssetLoader( const AssetLoader& ) = delete;
	AssetLoader( AssetLoader&& ) = delete;
	AssetLoader& operator=( const AssetLoader& ) = delete;
	AssetLoader& operator=( AssetLoader&& ) = delete;

private:
	AssetLoaderSharedHandle LoadAsset( const char* assetPath, LoadCompletionCallback completionCallback );

	void OnAssetLoaded( const std::string& path, const std::shared_ptr<void>& asset );
	void AddPrerequisiteToDependantAsset( const AssetLoaderSharedHandle& handle );

	std::unordered_map<std::string, std::vector<AssetLoaderSharedHandle>> m_waitingHandle;
	std::unordered_map<std::string, std::shared_ptr<void>> m_assets;

	thread_local static AssetLoaderSharedHandle AssetHandleInProcess;
};

thread_local AssetLoaderSharedHandle AssetLoader::AssetHandleInProcess = nullptr;

AssetLoaderSharedHandle AssetLoader::RequestAsyncLoad( const std::string& assetPath, LoadCompletionCallback completionCallback )
{
	assert( IsInGameThread() );

	auto found = m_assets.find( assetPath );
	if ( found != m_assets.end() )
	{
		auto handle = std::make_shared<AssetLoaderHandle>();
		handle->BindCompletionCallback( completionCallback );
		handle->SetLoadedAsset( found->second );
		handle->OnStartLoading();
		AddPrerequisiteToDependantAsset( handle );
		handle->ExecuteCompletionCallback();

		return handle;
	} 

	auto isInProgress = m_waitingHandle.find( assetPath );
	if ( isInProgress != m_waitingHandle.end() )
	{
		auto handle = std::make_shared<AssetLoaderHandle>();
		handle->BindCompletionCallback( completionCallback );
		handle->OnStartLoading();
		AddPrerequisiteToDependantAsset( handle );
		isInProgress->second.emplace_back( handle );
		return handle;
	}

	m_waitingHandle[assetPath];

	IAssetLoader::LoadCompletionCallback onAssetLoaded;
	onAssetLoaded.BindFunctor(
		[assetPath, completionCallback, this]( const std::shared_ptr<void>& asset )
		{
			if ( completionCallback.IsBound() )
			{
				completionCallback( asset );
			}
			OnAssetLoaded( assetPath, asset );
		}
	);

	AssetLoaderSharedHandle handle = LoadAsset( assetPath.c_str(), onAssetLoaded );

	assert( handle->IsLoadingInProgress() );
	return handle;
}

AssetLoaderSharedHandle AssetLoader::HandleInProcess() const
{
	return AssetHandleInProcess;
}

void AssetLoader::SetHandleInProcess( const AssetLoaderSharedHandle& handle )
{
	AssetHandleInProcess = handle;
}

AssetLoaderSharedHandle AssetLoader::LoadAsset( const char* assetPath, LoadCompletionCallback completionCallback )
{
	AssetLoaderSharedHandle handle = std::make_shared<AssetLoaderHandle>();

	IFileSystem* fileSystem = GetInterface<IFileSystem>();
	FileHandle hAsset = fileSystem->OpenFile( assetPath );
	
	if ( hAsset.IsValid() == false )
	{
		return handle;
	}

	uint32 assetSize = fileSystem->GetFileSize( hAsset );
	if ( assetSize == 0 )
	{
		return handle;
	}

	char* asset = new char[assetSize];

	IFileSystem::IOCompletionCallback AssetProcessing;
	AssetProcessing.BindFunctor(
		[this, handle, hAsset, path = std::filesystem::path( assetPath )](char*& buffer, uint32 bufferSize)
		{
			EnqueueThreadTask<WorkerThreads>(
				[this, buffer, bufferSize, handle, path = std::move( path )]()
				{
					SetHandleInProcess( handle );

					Archive ar( buffer, bufferSize );
					uint32 assetID = 0;

					ar << assetID;
					
					std::shared_ptr<IAsyncLoadableAsset> newAsset( GetInterface<IAssetFactory>()->CreateAsset( assetID ) );
					if ( newAsset != nullptr )
					{
						handle->SetLoadedAsset( newAsset );
						newAsset->SetPath( path );
						newAsset->Serialize( ar );

						if ( handle->HasPrerequisites() == false )
						{
							EnqueueThreadTask<ThreadType::GameThread>(
								[handle, newAsset]()
							{
								assert( IsInGameThread() );
								handle->ExecuteCompletionCallback();
							} );
						}
					}
					else // Fail...
					{

					}

					SetHandleInProcess( nullptr );
					delete[] buffer;
				} );

			buffer = nullptr;
			GetInterface<IFileSystem>()->CloseFile( hAsset );
		}
	);

	handle->BindCompletionCallback( completionCallback );
	handle->OnStartLoading();
	handle->NeedPostProcess();
	AddPrerequisiteToDependantAsset( handle );

	if ( fileSystem->ReadAsync( hAsset, asset, assetSize, &AssetProcessing ) == false )
	{
		delete[] asset;
		GetInterface<IFileSystem>()->CloseFile( hAsset );
	}

	return handle;
}

void AssetLoader::OnAssetLoaded( const std::string& path, const std::shared_ptr<void>& asset )
{
	assert( IsInGameThread() );

	auto result = m_assets.emplace( path, asset );
	assert( result.second );

	auto found = m_waitingHandle.find( path );
	if ( found != m_waitingHandle.end() )
	{
		for ( auto& handle : found->second )
		{
			handle->SetLoadedAsset( asset );
			handle->ExecuteCompletionCallback();
		}

		m_waitingHandle.erase( path );
	}
}

void AssetLoader::AddPrerequisiteToDependantAsset( const AssetLoaderSharedHandle& handle )
{
	if ( ( AssetHandleInProcess != nullptr ) && handle->IsLoadingInProgress() )
	{
		AssetHandleInProcess->AddPrerequisite( handle );
	}
}

Owner<IAssetLoader*> CreateAssetLoader()
{
	return new AssetLoader();
}

void DestoryAssetLoader( Owner<IAssetLoader*> pAssetLoader )
{
	delete pAssetLoader;
}
