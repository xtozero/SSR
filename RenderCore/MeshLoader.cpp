#include "stdafx.h"
#include "BaseMesh.h"
#include "IMeshLoader.h"
#include "MeshLoader.h"
#include "ObjMeshLoader.h"
#include "PlyMeshLoader.h"
#include "../shared/Util.h"

#include <assert.h>
#include <fstream>
#include <vector>

bool CMeshLoader::Initialize( )
{
	try
	{
		m_meshLoaders.emplace( String( _T( "ply" ) ), std::make_unique<CPlyMeshLoader>( ) );
		m_meshLoaders.emplace( String( _T( "obj" ) ), std::make_unique<CObjMeshLoader>( ) );
	}
	catch ( std::exception& )
	{
		return false;
	}

	return true;
}

bool CMeshLoader::LoadMeshFromFile( IRenderer& renderer, const TCHAR* pfileName, CSurfaceManager* pSurfaceManager )
{
	auto iter = m_meshList.find( pfileName );

	if ( iter != m_meshList.end( ) )
	{
		return true;
	}

	String exten = UTIL::FileNameExtension( pfileName );

	auto found = m_meshLoaders.find( exten );

	if ( found != m_meshLoaders.end() )
	{
		if ( found->second == nullptr )
		{
			return false;
		}

		IMesh* newMesh = found->second->LoadMeshFromFile( renderer, pfileName, pSurfaceManager );

		if ( newMesh == nullptr )
		{
			//Do Nothing
		}
		else
		{
			m_meshList.emplace( String( pfileName ), newMesh );
			return true;
		}
	}

	return false;
}

IMesh* CMeshLoader::GetMesh( const TCHAR* pfileName )
{
	auto iter = m_meshList.find( pfileName );

	if ( iter != m_meshList.end( ) )
	{
		return iter->second.get();
	}
	else
	{
		return nullptr;
	}
}

void CMeshLoader::RegisterMesh( const String& pMeshName, std::unique_ptr<IMesh> pMesh )
{
	if ( pMesh )
	{
		m_meshList.emplace( pMeshName, std::move( pMesh ) );
	}
}
