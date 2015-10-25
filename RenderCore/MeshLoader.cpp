#include "stdafx.h"
#include <assert.h>
#include "BaseMesh.h"
#include <D3DX9math.h>
#include <fstream>
#include "MeshLoader.h"
#include "PlyMesh.h"
#include <vector>

bool CMeshLoader::LoadMeshFromFile( const TCHAR* pfileName )
{
	std::ifstream meshfile( pfileName, 0 );

	UINT nVerties = 0;
	UINT index = 0;
	D3DXVECTOR3* vertices;

	if ( meshfile.is_open( ) )
	{
		char token[256] = { 0 };
		while ( meshfile.good( ) )
		{
			meshfile >> token;

			int symbollen = sizeof( "#$" ) - 1;
			if ( strncmp( token, "#$", symbollen ) == 0 )
			{
				if ( strncmp( token + symbollen, "Vertices", sizeof( "Vertices" ) ) == 0 )
				{
					meshfile >> nVerties;

					if ( nVerties <= 0 )
					{
						return false;
					}

					vertices = new D3DXVECTOR3[nVerties];
				}
			}
			else
			{
				if ( strncmp( token, "Vertex", sizeof( "Vertex" ) ) == 0 )
				{
					assert( index < nVerties );

					meshfile >> token;
					D3DXVECTOR3& vertex = vertices[index];

					meshfile >> vertex.x >> vertex.x >> vertex.z;
					++index;
				}
			}
		}

		auto iter = m_meshList.emplace( pfileName, std::make_shared<CPlyMesh>() );
		
		try
		{
			std::shared_ptr<BaseMesh> newMesh = m_meshList.at( pfileName );

			newMesh->SetModelData( vertices, nVerties );
			newMesh->Load( );
		}
		catch ( std::out_of_range e )
		{
			return false;
		}

		return true;
	}
	else
	{
		return false;
	}
}

CMeshLoader::CMeshLoader( )
{
}


CMeshLoader::~CMeshLoader( )
{
}
