#include "stdafx.h"
#include "Model/ModelManager.h"

#include "Model/BaseMesh.h"
#include "Model/IModelLoader.h"
#include "Util.h"

#include <assert.h>
#include <fstream>
#include <vector>

void CModelManager::OnDeviceRestore( CGameLogic & gameLogic )
{
	m_meshList.clear( );
}

IMesh* CModelManager::LoadMeshFromFile( IRenderer& renderer, const TCHAR* pfileName )
{
	auto iter = m_meshList.find( pfileName );

	if ( iter != m_meshList.end( ) )
	{
		return iter->second.get();
	}

	String exten = UTIL::FileNameExtension( pfileName );

	auto found = m_meshLoaders.find( exten );

	if ( found != m_meshLoaders.end() )
	{
		IMesh* newMesh = found->second.LoadMeshFromFile( renderer, pfileName, m_surfaces );

		if ( newMesh )
		{
			newMesh->SetName( pfileName );
			m_meshList.emplace( String( pfileName ), newMesh );
			return newMesh;
		}
	}

	return nullptr;
}

IMesh* CModelManager::FindModel( const String& modelName )
{
	auto found = m_meshList.find( modelName );

	if ( found != m_meshList.end( ) )
	{
		return found->second.get();
	}

	return nullptr;
}

void CModelManager::RegisterMesh( const String& modelName, std::unique_ptr<IMesh> pMesh )
{
	if ( pMesh )
	{
		pMesh->SetName( modelName.c_str( ) );
		m_meshList.emplace( modelName, std::move( pMesh ) );
	}
}
