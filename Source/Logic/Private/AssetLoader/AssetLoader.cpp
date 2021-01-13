#include "stdafx.h"
#include "AssetLoader/AssetLoader.h"

#include "Archive.h"
#include "Core/InterfaceFactories.h"
#include "FileSystem/EngineFileSystem.h"
#include "Rendering/RenderOption.h"
#include "Json/json.hpp"

#include <memory>
#include <unordered_map>

class AssetLoader : public IAssetLoader
{
public:
	virtual AssetLoaderSharedHandle RequestAsyncLoad( const std::string& assetPath, LoadCompletionCallback completionCallback ) override;

	AssetLoader( ) = default;
	AssetLoader( const AssetLoader& ) = delete;
	AssetLoader( AssetLoader&& ) = delete;
	AssetLoader& operator=( const AssetLoader& ) = delete;
	AssetLoader& operator=( AssetLoader&& ) = delete;

private:
	AssetLoaderSharedHandle LoadAsset( const char* assetPath, LoadCompletionCallback completionCallback );

	void OnAssetLoaded( const std::string& path, const std::shared_ptr<void>& asset );

	std::unordered_map<std::string, std::vector<AssetLoaderSharedHandle>> m_waitingHandle;
	std::unordered_map<std::string, std::shared_ptr<void>> m_assets;

	AssetLoaderSharedHandle m_dependantAssetHandle = nullptr;
};

AssetLoaderSharedHandle AssetLoader::RequestAsyncLoad( const std::string& assetPath, LoadCompletionCallback completionCallback )
{
	auto found = m_assets.find( assetPath );
	if ( found != m_assets.end() )
	{
		auto handle = std::make_shared<AssetLoaderHandle>( );
		handle->SetLoadedAsset( found->second );
		handle->ExecuteCompletionCallback( );
		return handle;
	} 

	auto isInProgress = m_waitingHandle.find( assetPath );
	if ( isInProgress != m_waitingHandle.end() )
	{
		auto handle = std::make_shared<AssetLoaderHandle>( );
		handle->BindCompletionCallback( completionCallback );
		handle->OnStartLoading( );
		isInProgress->second.emplace_back( handle );
		return handle;
	}

	m_waitingHandle[assetPath];

	IAssetLoader::LoadCompletionCallback onRenderOptionLoaded;
	onRenderOptionLoaded.BindFunctor(
		[assetPath, completionCallback, this]( const std::shared_ptr<void>& asset )
		{
			if ( completionCallback.IsBound( ) )
			{
				completionCallback( asset );
			}
			OnAssetLoaded( assetPath, asset );
		}
	);

	AssetLoaderSharedHandle handle = LoadAsset( assetPath.c_str(), onRenderOptionLoaded );

	if ( ( m_dependantAssetHandle != nullptr ) && handle->IsLoadingInProgress( ) )
	{
		m_dependantAssetHandle->AddPrerequisite( handle );
	}

	assert( handle->IsLoadingInProgress( ) );
	return handle;
}

AssetLoaderSharedHandle AssetLoader::LoadAsset( const char* assetPath, LoadCompletionCallback completionCallback )
{
	AssetLoaderSharedHandle handle = std::make_shared<AssetLoaderHandle>( );

	IFileSystem* fileSystem = GetInterface<IFileSystem>( );
	FileHandle hAsset = fileSystem->OpenFile( assetPath );
	
	if ( hAsset.IsValid( ) == false )
	{
		return handle;
	}

	unsigned long assetSize = fileSystem->GetFileSize( hAsset );
	if ( assetSize == 0 )
	{
		return handle;
	}

	char* asset = new char[assetSize];

	IFileSystem::IOCompletionCallback AssetProcessing;
	AssetProcessing.BindFunctor(
		[this, handle, hAsset, path = std::string( assetPath )]( const char* buffer, unsigned long bufferSize )
		{
			m_dependantAssetHandle = handle;

			Archive ar( buffer, bufferSize );
			int assetID = -1;

			ar << assetID;

			std::shared_ptr<IAsyncLoadableAsset> newAsset( AssetFactory::GetInstance( ).CreateAsset( assetID ) );
			if ( newAsset != nullptr )
			{
				newAsset->Serialize( ar );
				handle->SetLoadedAsset( newAsset );
				handle->ExecuteCompletionCallback( );
			}
			else // Fail...
			{

			}

			m_dependantAssetHandle = nullptr;

			GetInterface<IFileSystem>( )->CloseFile( hAsset );
		}
	);

	if ( fileSystem->ReadAsync( hAsset, asset, assetSize, &AssetProcessing ) == false )
	{
		delete[] asset;
		GetInterface<IFileSystem>( )->CloseFile( hAsset );
	}

	handle->BindCompletionCallback( completionCallback );
	handle->OnStartLoading( );

	return handle;
}

void AssetLoader::OnAssetLoaded( const std::string& path, const std::shared_ptr<void>& asset )
{
	auto result = m_assets.emplace( path, asset );
	assert( result.second );

	auto found = m_waitingHandle.find( path );
	if ( found != m_waitingHandle.end( ) )
	{
		for ( auto& handle : found->second )
		{
			handle->SetLoadedAsset( asset );
			handle->ExecuteCompletionCallback( );
		}

		m_waitingHandle.erase( found );
	}
}

Owner<IAssetLoader*> CreateAssetLoader( )
{
	IAssetLoader* pRenderOptionManager = new AssetLoader;
	return pRenderOptionManager;
}

void DestoryAssetLoader( Owner<IAssetLoader*> pRenderOptionManager )
{
	delete pRenderOptionManager;
}
