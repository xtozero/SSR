#include "stdafx.h"
#include "Model/PlyMeshLoader.h"

#include "Model/CommonMeshDefine.h"
#include "Model/PlyMesh.h"

#include <algorithm>
#include <assert.h>
#include <cstddef>
#include <fstream>
#include <vector>

using namespace DirectX;

namespace
{
	constexpr std::size_t PLY_FILE_READ_INDEX_STEP = 3;

	constexpr TCHAR* PLY_FILE_DIR = _T( "../model/ply/" );
	constexpr TCHAR* PLY_DEFAULT_SURFACE_NAME = _T( "PlyDefaultSurface" );

	void CalcPlyNormal( MeshVertex* vertices, const UINT vertexCount, const WORD* indices, const UINT indexCount )
	{
		std::vector<CXMFLOAT3> normals( vertexCount );

		for ( CXMFLOAT3& normal : normals )
		{
			normal.x = 0.f;
			normal.y = 0.f;
			normal.z = 0.f;
		}

		std::vector<UINT> idxList;
		idxList.reserve( std::max( vertexCount, indexCount ) );

		if ( indexCount != 0 )
		{
			for ( UINT i = 0; i < indexCount; ++i )
			{
				idxList.push_back( indices[i] );
			}
		}
		else
		{
			for ( UINT i = 0; i < vertexCount; ++i )
			{
				idxList.push_back( i );
			}
		}

		for ( std::size_t i = 0; i < idxList.size(); i += 3 )
		{
			const CXMFLOAT3& p0 = vertices[idxList[i]].m_position;
			const CXMFLOAT3& p1 = vertices[idxList[i + 1]].m_position;
			const CXMFLOAT3& p2 = vertices[idxList[i + 2]].m_position;

			const CXMFLOAT3& v0 = p1 - p0;
			const CXMFLOAT3& v1 = p2 - p0;

			CXMFLOAT3 normal;

			normal = XMVector3Cross( v0, v1 );

			normals[idxList[i]] += normal;
			normals[idxList[i + 1]] += normal;
			normals[idxList[i + 2]] += normal;
		}

		int idx = 0;
		for ( CXMFLOAT3& normal : normals )
		{
			vertices[idx].m_normal = XMVector3Normalize( normal );
			++idx;
		}
	}
}

Owner<IMesh*> CPlyMeshLoader::LoadMeshFromFile( IRenderer& renderer, const TCHAR* pFileName, SurfaceMap& /*surface*/ )
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
	CXMFLOAT3 randColor( static_cast<float>( rand( ) ) / RAND_MAX,
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
				CXMFLOAT3& vertex = vertices[vertexIdx].m_position;

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

	CalcPlyNormal( vertices, vertexCount, indices, indexCount );

	CPlyMesh* newMesh = new CPlyMesh;

	newMesh->SetModelData( vertices, vertexCount );
	newMesh->SetIndexData( indices, indexCount );
	
	if ( newMesh->Load( renderer ) )
	{
		return newMesh;
	}
	else
	{
		return nullptr;
	}
}
