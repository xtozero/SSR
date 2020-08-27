#include "stdafx.h"
#include "Model/ModelManager.h"

#include "Model/IModelLoader.h"
#include "Model/MeshDescription.h"
#include "Util.h"

#include <assert.h>
#include <fstream>
#include <vector>

void CModelManager::OnDeviceRestore( CGameLogic& /*gameLogic*/ )
{
	// m_modelList.clear( );
}

MeshDescription* CModelManager::RequestAsyncLoad( const char* pFilePath, LoadCompletionCallback completionCallback )
{
	//auto iter = m_modelList.find( pFilePath );

	//if ( iter != m_modelList.end( ) )
	//{
	//	return iter->second.get( );
	//}

	std::string exten = UTIL::FileNameExtension( pFilePath );

	auto found = m_meshLoaders.find( exten );

	if ( found != m_meshLoaders.end() )
	{
		IModelLoader::LoadCompletionCallback postMeshLoading;
		postMeshLoading.BindFunctor( 
			[completionCallback, this]( MeshDescription&& meshDescription, std::vector<Material>&& materials )
			{
				void* asset = PostMeshLoading( std::move( meshDescription ), std::move( materials ) );
				if ( completionCallback.IsBound() )
				{
					completionCallback( asset );
				}
			}
		);

		MeshDescription* newMesh = found->second.RequestAsyncLoad( pFilePath, postMeshLoading );

		if ( newMesh )
		{
			//m_modelList.emplace( pFilePath, newMesh );
			return newMesh;
		}
	}

	return nullptr;
}

void CModelManager::RegisterMesh( const std::string& modelName, Owner<MeshDescription*> pMesh )
{
	if ( pMesh )
	{
		//m_modelList.emplace( modelName, std::move( pMesh ) );
	}
}

void* CModelManager::PostMeshLoading( MeshDescription&& meshDescription, std::vector<Material>&& materials )
{
	return nullptr;
}
