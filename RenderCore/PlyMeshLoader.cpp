#include "stdafx.h"
#include <assert.h>
#include "CommonMeshDefine.h"
#include <d3dX9math.h>
#include <fstream>
#include "PlyMesh.h"
#include "PlyMeshLoader.h"
#include <vector>

namespace
{
	const int PLY_FILE_READ_INDEX_STEP = 3;

	const TCHAR* PLY_FILE_DIR = _T( "../model/ply/" );
}

std::shared_ptr<IMesh> CPlyMeshLoader::LoadMeshFromFile( const TCHAR* pFileName )
{
	TCHAR pPath[MAX_PATH];
	::GetCurrentDirectory( MAX_PATH, pPath );
	::SetCurrentDirectory( PLY_FILE_DIR );

	std::ifstream meshfile( pFileName, 0 );

	UINT vertexCount = 0;
	UINT indexCount = 0;
	MeshVertex* vertices = nullptr;
	WORD* indices = nullptr;

	UINT vertexIdx = 0;
	UINT indexIdx = 0;

	if ( !meshfile.is_open( ) )
	{
		::SetCurrentDirectory( pPath );
		return nullptr;
	}

	char token[256] = { 0 };
	D3DXVECTOR3 randColor( static_cast<float>( rand( ) ) / RAND_MAX,
		static_cast<float>( rand( ) ) / RAND_MAX,
		static_cast<float>( rand( ) ) / RAND_MAX );

	while ( meshfile.good( ) )
	{
		meshfile >> token;

		int symbollen = sizeof( "#$" ) - 1;
		if ( strncmp( token, "#$", symbollen ) == 0 )
		{
			if ( strncmp( token + symbollen, "Vertices", strlen( "Vertices" ) ) == 0 )
			{
				meshfile >> vertexCount;

				if ( vertexCount <= 0 )
				{
					return false;
				}

				vertices = new MeshVertex[vertexCount];
			}
			else if ( strncmp( token + symbollen, "Faces", strlen( "Faces" ) ) == 0 )
			{
				meshfile >> indexCount;

				if ( indexCount > 0 )
				{
					indexCount *= PLY_FILE_READ_INDEX_STEP;
					indices = new WORD[indexCount];
				}
			}
		}
		else
		{
			if ( strncmp( token, "Vertex", strlen( "Vertex" ) ) == 0 )
			{
				assert( vertexIdx < vertexCount );

				meshfile >> token;
				D3DXVECTOR3& vertex = vertices[vertexIdx].m_position;

				meshfile >> vertex.x >> vertex.y >> vertex.z;
				vertices[vertexIdx].m_color = randColor;
				++vertexIdx;
			}
			else if ( strncmp( token, "Face", strlen( "Face" ) ) == 0 )
			{
				assert( indexIdx + PLY_FILE_READ_INDEX_STEP <= indexCount );

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

	::SetCurrentDirectory( pPath );
	meshfile.close( );

	auto newMesh = std::make_shared<CPlyMesh>( );

	newMesh->SetModelData( vertices, vertexCount );
	newMesh->SetIndexData( indices, indexCount );
	
	if ( newMesh->Load( ) )
	{
		return newMesh;
	}
	else
	{
		return nullptr;
	}
}

CPlyMeshLoader::CPlyMeshLoader( )
{
}


CPlyMeshLoader::~CPlyMeshLoader( )
{
}
