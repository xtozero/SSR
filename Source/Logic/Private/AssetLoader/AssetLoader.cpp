#include "stdafx.h"
#include "AssetLoader/AssetLoader.h"

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

	void OnAssetLoaded( const std::string& path, void* asset );

	std::unordered_map<std::string, std::vector<AssetLoaderSharedHandle>> m_waitingHandle;
	std::unordered_map<std::string, std::shared_ptr<IAsyncLoadableAsset>> m_assets;
	std::unordered_map<std::string, void*> m_loaders;
};

AssetLoaderSharedHandle AssetLoader::RequestAsyncLoad( const std::string& assetPath, LoadCompletionCallback completionCallback )
{
	auto found = m_assets.find( assetPath );
	if ( found != m_assets.end() )
	{
		auto handle = std::make_shared<AssetLoaderHandle>( );
		handle->SetLoadedAsset( found->second.get( ) );
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
		[assetPath, completionCallback, this]( void* asset )
		{
			if ( completionCallback.IsBound( ) )
			{
				completionCallback( asset );
			}
			OnAssetLoaded( assetPath, asset );
		}
	);

	AssetLoaderSharedHandle handle = LoadAsset( assetPath.c_str(), onRenderOptionLoaded );

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
	if ( strstr( assetPath, ".asset" ) )
	{
		AssetProcessing.BindFunctor(
			[this, handle, hAsset]( const char* buffer, unsigned long bufferSize )
			{
				JSON::Value assetJson;
				JSON::Reader reader;
				if ( reader.Parse( buffer, bufferSize, assetJson ) == false )
				{
					__debugbreak( );
				}

				if ( const JSON::Value* pType = assetJson.Find( "Type" ) )
				{
					IAsyncLoadableAsset* newAsset = AssetFactory::GetInstance( ).CreateAsset( pType->AsString( ) );

					assert( newAsset != nullptr );

					if ( newAsset )
					{
						newAsset->LoadFromAsset( assetJson, handle );
						handle->SetLoadedAsset( newAsset );
					}
				}

				handle->ExecuteCompletionCallback( );
				GetInterface<IFileSystem>( )->CloseFile( hAsset );
			}
		);
	}
	else if ( const char* assetTypeStart = strrchr( assetPath, '/' ) )
	{	
		if ( const char* assetTypeEnd = strchr( assetTypeStart, '_' ) )
		{
			std::string type( assetTypeStart + 1, assetTypeEnd );

			AssetProcessing.BindFunctor(
				[this, handle, hAsset, type]( const char* buffer, unsigned long bufferSize )
				{
					IAsyncLoadableAsset* newAsset = AssetFactory::GetInstance( ).CreateAsset( type );

					assert( newAsset != nullptr );

					if ( newAsset )
					{
						newAsset->LoadFromAsset( buffer, bufferSize );
						handle->SetLoadedAsset( newAsset );
					}

					handle->ExecuteCompletionCallback( );
					GetInterface<IFileSystem>( )->CloseFile( hAsset );
				}
			);
		}
	}
	

	if ( fileSystem->ReadAsync( hAsset, asset, assetSize, &AssetProcessing ) == false )
	{
		delete[] asset;
		GetInterface<IFileSystem>( )->CloseFile( hAsset );
	}

	handle->BindCompletionCallback( completionCallback );
	handle->OnStartLoading( );

	return handle;
}

void AssetLoader::OnAssetLoaded( const std::string& path, void* asset )
{
	auto result = m_assets.emplace( path, static_cast<IAsyncLoadableAsset*>( asset ) );
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
