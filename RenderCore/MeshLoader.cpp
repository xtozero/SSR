#include "stdafx.h"
#include <assert.h>
#include "BaseMesh.h"
#include <d3dx9math.h>
#include <fstream>
#include "IMeshLoader.h"
#include "MeshLoader.h"
#include "ObjMeshLoader.h"
#include "PlyMeshLoader.h"
#include "../shared/Util.h"
#include <vector>

bool CMeshLoader::Initialize( )
{
	try
	{
		m_meshLoaders.emplace( String( _T( "ply" ) ), std::make_unique<CPlyMeshLoader>( ) );
		m_meshLoaders.emplace( String( _T( "obj" ) ), std::make_unique<CObjMeshLoader>( ) );
	}
	catch ( std::exception e )
	{
		return false;
	}

	return true;
}

bool CMeshLoader::LoadMeshFromFile( const TCHAR* pfileName )
{
	auto iter = m_meshList.find( pfileName );

	if ( iter != m_meshList.end( ) )
	{
		return true;
	}

	String exten = UTIL::FileNameExtension( pfileName );
	std::shared_ptr<IMesh> newMesh = nullptr;

	auto found = m_meshLoaders.find( exten );

	if ( found != m_meshLoaders.end() )
	{
		if ( found->second == nullptr )
		{
			return false;
		}

		newMesh = found->second->LoadMeshFromFile( pfileName );

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

std::shared_ptr<IMesh> CMeshLoader::GetMesh( const TCHAR* pfileName )
{
	auto iter = m_meshList.find( pfileName );

	if ( iter != m_meshList.end( ) )
	{
		return iter->second;
	}
	else
	{
		return nullptr;
	}
}

CMeshLoader::CMeshLoader( )
{
}


CMeshLoader::~CMeshLoader( )
{
}
