#include "stdafx.h"
#include <assert.h>
#include "BaseMesh.h"
#include <D3DX9math.h>
#include <fstream>
#include "MeshLoader.h"
#include "PlyMesh.h"
#include <vector>

namespace
{
	const int PLY_FILE_READ_INDEX_STEP = 3;
}

bool CMeshLoader::LoadMeshFromFile( const TCHAR* pfileName )
{
	auto iter = m_meshList.find( pfileName );

	if ( iter != m_meshList.end( ) )
	{
		return true;
	}

	std::ifstream meshfile( pfileName, 0 );

	UINT nVerties = 0;
	UINT nIndices = 0;
	D3DXVECTOR3* vertices = nullptr;
	WORD* indices = nullptr;

	UINT vextexIdx = 0;
	UINT indexIdx = 0;

	if ( meshfile.is_open( ) )
	{
		char token[256] = { 0 };
		while ( meshfile.good( ) )
		{
			meshfile >> token;

			int symbollen = sizeof( "#$" ) - 1;
			if ( strncmp( token, "#$", symbollen ) == 0 )
			{
				if ( strncmp( token + symbollen, "Vertices", strlen( "Vertices" ) ) == 0 )
				{
					meshfile >> nVerties;

					if ( nVerties <= 0 )
					{
						return false;
					}

					vertices = new D3DXVECTOR3[nVerties];
				}
				else if ( strncmp( token + symbollen, "Faces", strlen( "Faces" ) ) == 0 )
				{
					meshfile >> nIndices;

					if ( nIndices > 0 )
					{
						nIndices *= PLY_FILE_READ_INDEX_STEP;
						indices = new WORD[nIndices];
					}
				}
			}
			else
			{
				if ( strncmp( token, "Vertex", strlen( "Vertex" ) ) == 0 )
				{
					assert( vextexIdx < nVerties );

					meshfile >> token;
					D3DXVECTOR3& vertex = vertices[vextexIdx];

					meshfile >> vertex.x >> vertex.y >> vertex.z;
					++vextexIdx;
				}
				else if ( strncmp( token, "Face", strlen( "Face" ) ) == 0 )
				{
					assert( indexIdx + PLY_FILE_READ_INDEX_STEP <= nIndices );

					meshfile >> token;
					int index;

					for ( UINT i = indexIdx; i < indexIdx + 3; ++i )
					{
						meshfile >> index;
						indices[i] = static_cast<WORD>( index - 1 );
					}

					indexIdx += PLY_FILE_READ_INDEX_STEP;
				}
			}
		}

		auto iter = m_meshList.emplace( pfileName, std::make_shared<CPlyMesh>() );
		
		try
		{
			std::shared_ptr<IMesh> newMesh = m_meshList.at( pfileName );

			newMesh->SetModelData( vertices, nVerties );
			newMesh->SetIndexData( indices, nIndices );
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
