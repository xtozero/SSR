#include "stdafx.h"
#include <assert.h>
#include "CommonMeshDefine.h"
#include <D3DX9math.h>
#include <fstream>
#include <iostream>
#include "PlyMesh.h"
#include "ObjMeshLoader.h"
#include "../shared/Util.h"
#include <vector>

namespace
{
	enum OBJ_FACE_ELEMENT
	{
		VERTEX = 0,
		TEXTURE,
		NORMAL,
	};
}

std::shared_ptr<IMesh> CObjMeshLoader::LoadMeshFromFile( const TCHAR* pFileName )
{
	std::wifstream meshFile( pFileName, 0 );

	std::vector<D3DXVECTOR3> positions;
	std::vector<D3DXVECTOR3> normals;
	std::vector<D3DXVECTOR2> texCoords;

	std::vector<int> facePositionInfo;
	std::vector<int> faceTexInfo;
	std::vector<int> faceNormalnfo;

	std::vector<MeshVertex> vertices;
	std::vector<WORD> indices;

	int nFaces = 0;

	if ( !meshFile.is_open( ) )
	{
		return nullptr;
	}
	
	String token;
	while ( meshFile.good( ) )
	{
		std::getline( meshFile, token );

		if ( token.compare( 0, 1, _T( "#" ) ) == 0 )
		{
			//Do Nothing
		}
		else if ( token.compare( 0, 2, _T( "vn" ) ) == 0 )
		{
			std::vector<String> params;
			UTIL::Split( token, params, _T( ' ' ) );

			assert( params.size( ) == 4 );

			normals.emplace_back( _ttof( params[1].c_str( ) ), _ttof( params[2].c_str( ) ), _ttof( params[3].c_str( ) ) );
		}
		else if ( token.compare( 0, 2, _T( "vt" ) ) == 0 )
		{
			std::vector<String> params;
			UTIL::Split( token, params, _T( ' ' ) );

			assert( params.size( ) == 3 );

			texCoords.emplace_back( _ttof( params[1].c_str( ) ), _ttof( params[2].c_str( ) ) );
		}
		else if ( token.compare( 0, 2, _T( "v " ) ) == 0 )
		{
			std::vector<String> params;
			UTIL::Split( token, params, _T( ' ' ) );

			assert( params.size( ) == 4 );

			positions.emplace_back( _ttof( params[1].c_str( ) ), _ttof( params[2].c_str( ) ), _ttof( params[3].c_str( ) ) );
		}
		else if ( token.compare( 0, 2, _T( "f " ) ) == 0 )
		{
			++nFaces;
			std::vector<String> params;
			UTIL::Split( token, params, _T( ' ' ) );

			assert( params.size( ) == 4 );

			for ( int i = 1; i < params.size( ); ++i )
			{
				std::vector<String> face;

				UTIL::Split( params[i], face, _T( '/' ) );

				for ( auto iter = face.begin( ); iter != face.end( ); ++iter )
				{
					int idx = _ttoi( iter->c_str( ) ) - 1;

					if ( idx != -1 )
					{
						switch ( std::distance( face.begin( ), iter ) )
						{
						case OBJ_FACE_ELEMENT::VERTEX:
							facePositionInfo.push_back( idx );
							break;
						case OBJ_FACE_ELEMENT::TEXTURE:
							faceTexInfo.push_back( idx );
							break;
						case OBJ_FACE_ELEMENT::NORMAL:
							faceNormalnfo.push_back( idx );
							break;
						}
					}
				}
			}
		}
	}

#ifdef TEST_CODE
	for ( auto iter = vertices.begin( ); iter != vertices.end( ); ++iter )
	{
		std::cout << "v:" << iter->x << ", "
			<< iter->y << ", "
			<< iter->z << std::endl;
	}

	for ( auto iter = normals.begin( ); iter != normals.end( ); ++iter )
	{
		std::cout << "n:" << iter->x << ", "
			<< iter->y << ", "
			<< iter->z << std::endl;
	}

	for ( auto iter = facePositionInfo.begin( ); iter != facePositionInfo.end( ); ++iter )
	{
		std::cout << "fv:" << *iter << std::endl;
	}

	for ( auto iter = faceTexInfo.begin( ); iter != faceTexInfo.end( ); ++iter )
	{
		std::cout << "ft:" << *iter << std::endl;
	}

	for ( auto iter = faceNormalnfo.begin( ); iter != faceNormalnfo.end( ); ++iter )
	{
		std::cout << "fn:" << *iter << std::endl;
	}
#endif

	//Test Code without optimizing Will be Changed
	MeshVertex curVertex;
	D3DXVECTOR3 randColor( static_cast<float>( rand( ) ) / RAND_MAX, 
							static_cast<float>( rand( ) ) / RAND_MAX,
							static_cast<float>( rand( ) ) / RAND_MAX );

	for ( int i = 0; i < nFaces * 3; ++i )
	{
		::ZeroMemory( &curVertex, VERTEX_STRIDE );
		if ( i < facePositionInfo.size( ) )
		{
			curVertex.m_position = positions[facePositionInfo[i]];
		}

		if ( i < faceNormalnfo.size( ) )
		{
			curVertex.m_normal = normals[faceNormalnfo[i]];
		}

		if ( i < faceTexInfo.size( ) )
		{
			curVertex.m_texcoord = texCoords[faceTexInfo[i]];
		}
		
		curVertex.m_color = randColor;

		vertices.push_back( curVertex );
	}
	
	for ( auto iter = vertices.begin( ); iter != vertices.end( ); ++iter )
	{
		indices.push_back( std::distance( vertices.begin( ), iter ) );
	}
	//

	auto newMesh = std::make_shared<CPlyMesh>( );

	try
	{
		newMesh->SetModelData( &vertices[0], vertices.size( ) );
		newMesh->SetIndexData( &indices[0], indices.size( ) );
		newMesh->Load( );

		return newMesh;
	}
	catch ( std::out_of_range e )
	{
		return nullptr;
	}

	return nullptr;
}

CObjMeshLoader::CObjMeshLoader( )
{
}


CObjMeshLoader::~CObjMeshLoader( )
{
}
