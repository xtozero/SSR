#include "stdafx.h"
#include "CommonMeshDefine.h"
#include "PlyMesh.h"
#include "PlyMeshLoader.h"
#include "Surface.h"
#include "SurfaceManager.h"

#include <assert.h>
#include <d3dX9math.h>
#include <fstream>
#include <vector>

namespace
{
	constexpr int PLY_FILE_READ_INDEX_STEP = 3;

	constexpr TCHAR* PLY_FILE_DIR = _T( "../model/ply/" );
	constexpr TCHAR* PLY_DEFAULT_SURFACE_NAME = _T( "PlyDefaultSurface" );

	void CalcPlyNormal( MeshVertex* vertices, const UINT vertexCount, const WORD* indices, const UINT indexCount )
	{
		std::vector<D3DXVECTOR3> normals( vertexCount );

		for ( D3DXVECTOR3& normal : normals )
		{
			normal.x = 0.f;
			normal.y = 0.f;
			normal.z = 0.f;
		}

		std::vector<UINT> idxList;
		idxList.reserve( max( vertexCount, indexCount ) );

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

		for ( UINT i = 0; i < idxList.size(); i += 3 )
		{
			const D3DXVECTOR3& p0 = vertices[idxList[i]].m_position;
			const D3DXVECTOR3& p1 = vertices[idxList[i + 1]].m_position;
			const D3DXVECTOR3& p2 = vertices[idxList[i + 2]].m_position;

			const D3DXVECTOR3& v0 = p1 - p0;
			const D3DXVECTOR3& v1 = p2 - p0;

			D3DXVECTOR3 normal;

			D3DXVec3Cross( &normal, &v0, &v1 );

			normals[idxList[i]] += normal;
			normals[idxList[i + 1]] += normal;
			normals[idxList[i + 2]] += normal;
		}

		int idx = 0;
		for ( D3DXVECTOR3& normal : normals )
		{
			D3DXVec3Normalize( &vertices[idx].m_normal, &normal );
			++idx;
		}
	}
}

std::shared_ptr<IMesh> CPlyMeshLoader::LoadMeshFromFile( const TCHAR* pFileName, CSurfaceManager* pSurfaceManager )
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

	CalcPlyNormal( vertices, vertexCount, indices, indexCount );

	auto newMesh = std::make_shared<CPlyMesh>( );

	newMesh->SetModelData( vertices, vertexCount );
	newMesh->SetIndexData( indices, indexCount );

	ISurface* defaultSurface = pSurfaceManager->FindSurface( PLY_DEFAULT_SURFACE_NAME );

	if ( defaultSurface == nullptr )
	{
		std::unique_ptr<ISurface> newSurface = std::make_unique<CSurface>( );
		defaultSurface = pSurfaceManager->RegisterSurface( PLY_DEFAULT_SURFACE_NAME, newSurface );
	}
	
	newMesh->SetSurface( defaultSurface );
	
	if ( newMesh->Load( ) )
	{
		return newMesh;
	}
	else
	{
		return nullptr;
	}
}
