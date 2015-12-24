#include "stdafx.h"
#include <assert.h>
#include "BaseMesh.h"
#include <D3DX9math.h>
#include <fstream>
#include "MeshLoader.h"
#include "ObjMeshLoader.h"
#include "PlyMeshLoader.h"
#include "../shared/Util.h"
#include <vector>

bool CMeshLoader::LoadMeshFromFile( const TCHAR* pfileName )
{
	auto iter = m_meshList.find( pfileName );

	if ( iter != m_meshList.end( ) )
	{
		return true;
	}

	String exten = UTIL::FileNameExtension( pfileName );
	std::shared_ptr<IMesh> newMesh = nullptr;

	if ( exten == String( _T( "ply" ) ) )
	{
		newMesh = CPlyMeshLoader::LoadMeshFromFile( pfileName );
	}
	else if ( exten == String( _T( "obj" ) ) )
	{
		newMesh = CObjMeshLoader::LoadMeshFromFile( pfileName );
	}


	if ( newMesh != nullptr )
	{
		m_meshList.emplace( String( pfileName ), newMesh );
		return true;
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
