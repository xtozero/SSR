#include "stdafx.h"
#include "Model/ModelManager.h"

#include "Model/IModelLoader.h"
#include "Model/MeshDescription.h"
#include "Model/StaticMesh.h"
#include "Util.h"

#include <assert.h>
#include <fstream>
#include <vector>

void CModelManager::OnDeviceRestore( CGameLogic& /*gameLogic*/ )
{
	// m_modelList.clear( );
}

ModelLoaderSharedHandle CModelManager::RequestAsyncLoad( const char* pFilePath, LoadCompletionCallback completionCallback )
{
	auto iter = m_modelList.find( pFilePath );
	if ( iter != m_modelList.end( ) )
	{
		auto handle = std::make_shared<ModelLoaderHandle>( );
		handle->SetLoadedAsset( iter->second.get() );
		handle->ExecuteCompletionCallback( );
		return handle;
	}

	std::string exten = UTIL::FileNameExtension( pFilePath );

	auto found = m_meshLoaders.find( exten );

	if ( found != m_meshLoaders.end() )
	{
		std::string filePath = pFilePath;

		IModelLoader::LoadCompletionCallback postMeshLoading;
		postMeshLoading.BindFunctor( 
			[path = std::move( filePath ), completionCallback, this]( void* asset )
			{
				PostMeshLoading( path, asset );
				if ( completionCallback.IsBound() )
				{
					completionCallback( asset );
				}
			}
		);

		ModelLoaderSharedHandle handle = found->second.RequestAsyncLoad( pFilePath, postMeshLoading );

		assert( handle->IsLoadingInProgress( ) );
		return handle;
	}

	return std::make_shared<ModelLoaderHandle>( );
}

void CModelManager::PostMeshLoading( const std::string& path, void* asset )
{
	auto result = m_modelList.emplace( path, static_cast<BaseMesh*>( asset ) );
	assert( result.second );
}
