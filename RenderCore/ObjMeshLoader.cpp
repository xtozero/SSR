#include "stdafx.h"

#include "CommonMeshDefine.h"
#include "ObjMesh.h"
#include "ObjMeshLoader.h"
#include "Surface.h"
#include "SurfaceManager.h"
#include "../shared/Util.h"

#include <assert.h>
#include <d3dX9math.h>
#include <fstream>
#include <iostream>

namespace
{
	enum OBJ_FACE_ELEMENT
	{
		VERTEX = 0,
		TEXTURE,
		NORMAL,
	};

	const TCHAR* OBJ_FILE_DIR = _T( "../model/obj/" );
}

std::shared_ptr<IMesh> CObjMeshLoader::LoadMeshFromFile( const TCHAR* pFileName, CSurfaceManager* pSurfaceManager )
{
	if ( pSurfaceManager == nullptr )
	{
		return nullptr;
	}

	Initialize( );
	TCHAR pPath[MAX_PATH];
	::GetCurrentDirectory( MAX_PATH, pPath );
	::SetCurrentDirectory( OBJ_FILE_DIR );

	Ifstream meshFile;
	meshFile.open( pFileName, 0 );

	if ( !meshFile.is_open( ) )
	{
		::SetCurrentDirectory( pPath );
		return nullptr;
	}
	
	String token;
	Stringstream sStream;

	while ( meshFile.good( ) )
	{
		sStream.str( _T( "" ) );
		sStream.clear( );

		std::getline( meshFile, token );

		std::vector<String> params;
		UTIL::Split( token, params, _T( ' ' ) );

		UINT count = static_cast<UINT>( params.size( ) );

		if ( token.compare( 0, 1, _T( "#" ) ) == 0 )
		{
			//Do Nothing
		}
		else if ( token.find( _T( "vn" ) ) != String::npos )
		{
			assert( count == 4 );

			sStream << params[1].c_str( ) << ' ' <<
				params[2].c_str( ) << ' ' <<
				params[3].c_str( );

			float x = 0.f, y = 0.f, z = 0.f;
			sStream >> x >> y >> z;

			m_normals.emplace_back( x, y, z );
		}
		else if ( token.find( _T( "vt" ) ) != String::npos )
		{
			assert( count == 3 );

			sStream << params[1].c_str( ) << ' ' <<
				params[2].c_str( );

			float u = 0.f, v = 0.f;
			sStream >> u >> v;

			m_texCoords.emplace_back( u, 1.0f - v );
		}
		else if ( token.find( _T( "v " ) ) != String::npos )
		{
			assert( count == 4 );

			sStream << params[1].c_str() << ' ' << 
				params[2].c_str( ) << ' ' <<
				params[3].c_str( );

			float x = 0.f, y = 0.f, z = 0.f;
			sStream >> x >> y >> z;

			assert( !sStream.fail( ) && !sStream.bad( ) && sStream.eof( ) );

 			m_positions.emplace_back( x, y, z );
		}
		else if ( token.find( _T( "f " ) ) != String::npos )
		{
			assert( count == 4 );

			for ( UINT i = 1; i < count; ++i )
			{
				std::vector<String> face;

				UTIL::Split( params[i], face, _T( '/' ) );
				int pos = -1;
				int tex = -1;
				int normal = -1;

				FOR_EACH_VEC( face, iter )
				{
					int idx = _ttoi( iter->c_str( ) ) - 1;

					if ( idx != -1 )
					{
						switch ( std::distance( face.begin( ), iter ) )
						{
						case OBJ_FACE_ELEMENT::VERTEX:
							pos = idx;
							break;
						case OBJ_FACE_ELEMENT::TEXTURE:
							tex = idx;
							break;
						case OBJ_FACE_ELEMENT::NORMAL:
							normal = idx;
							break;
						}
					}
				}

				m_faceInfo.emplace_back( pos, tex, normal );
			}

			auto iter = m_faceMtlGroup.rbegin( );
			if ( iter != m_faceMtlGroup.rend( ) )
			{
				iter->m_endFaceIndex = max( m_faceInfo.size( ) - 1, 0 );
			}
		}
		else if ( token.find( _T( "mtllib" ) ) != String::npos )
		{
			if ( count > 1 )
			{
				LoadMaterialFile( params[1].c_str(), pSurfaceManager );
			}
		}
		else if ( token.find( _T( "usemtl" ) ) != String::npos )
		{
			if ( count > 1 )
			{
				m_faceMtlGroup.emplace_back( 0, params[1] );
			}
		}
	}

	::SetCurrentDirectory( pPath );
	meshFile.close();

#ifdef TEST_CODE
	for ( auto iter = m_vertices.begin( ); iter != m_vertices.end( ); ++iter )
	{
		DebugMsg( "v: %f, %f, %f\n", iter->x, iter->y, iter->z );
	}

	for ( auto iter = m_normals.begin( ); iter != m_normals.end( ); ++iter )
	{
		DebugMsg( "n: %f, %f, %f\n", iter->x, iter->y, iter->z );
	}
#endif

	const std::vector<MeshVertex>& buildedVertices = BuildVertices( );
	std::vector<WORD> buildedindices;
	
	FOR_EACH_VEC( buildedVertices, iter )
	{
		buildedindices.push_back( static_cast<WORD>( std::distance( buildedVertices.begin( ), iter ) ) );
	}

	int vertexCount = buildedVertices.size( );
	int indexCount = buildedindices.size( );

	MeshVertex* vertices = new MeshVertex[vertexCount];
	WORD* indices = new WORD[indexCount];

	::memcpy_s( vertices, sizeof(MeshVertex) * vertexCount, &buildedVertices[0], sizeof(MeshVertex)* vertexCount );
	::memcpy_s( indices, sizeof(WORD) * indexCount, &buildedindices[0], sizeof(WORD)* indexCount );

	auto newMesh = std::make_shared<CObjMesh>( );

	FOR_EACH_VEC( m_mtlGroup, i )
	{
		ObjSurfaceTrait trait;

		trait.m_indexOffset = i->m_startIndex;
		trait.m_indexCount = i->m_endIndex - i->m_startIndex + 1;

		ISurface* surface = pSurfaceManager->FindSurface( i->m_materialName );
		trait.m_pSurface = surface;

		newMesh->AddMaterialGroup( trait );
	}
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

void CObjMeshLoader::Initialize( )
{
	m_positions.clear();
	m_normals.clear( );
	m_texCoords.clear( );

	m_faceInfo.clear( );

	m_faceMtlGroup.clear( );
	m_mtlGroup.clear( );
}

std::vector<MeshVertex> CObjMeshLoader::BuildVertices( )
{
	std::vector<MeshVertex> vertices;

	MeshVertex curVertex;
	D3DXVECTOR3 randColor( static_cast<float>( rand( ) ) / RAND_MAX,
		static_cast<float>( rand( ) ) / RAND_MAX,
		static_cast<float>( rand( ) ) / RAND_MAX );

	UINT count = m_faceInfo.size( );

	UINT startGroupIdx = 0;
	auto iter = m_faceMtlGroup.begin( );

	for ( UINT i = 0; i < count; ++i )
	{
		::ZeroMemory( &curVertex, VERTEX_STRIDE );

		const ObjFaceInfo &info = m_faceInfo[i];

		if ( info.m_position != -1 )
		{
			curVertex.m_position = m_positions[info.m_position];
		}

		if ( info.m_normal != -1 )
		{
			curVertex.m_normal = m_normals[info.m_normal];
		}

		if ( info.m_texCoord != -1 )
		{
			curVertex.m_texcoord = m_texCoords[info.m_texCoord];
		}

		curVertex.m_color = randColor;

		vertices.push_back( curVertex );

		if ( iter != m_faceMtlGroup.end() && i == iter->m_endFaceIndex )
		{
			m_mtlGroup.emplace_back( startGroupIdx, i, iter->m_materialName );
			startGroupIdx = i + 1;
			++iter;
		}
	}

	return vertices;
}

void CObjMeshLoader::LoadMaterialFile( const TCHAR* pFileName, CSurfaceManager* pSurfaceManager )
{
	if ( pSurfaceManager == nullptr )
	{
		return;
	}

	Ifstream materialFile( pFileName, 0 );

	if ( !materialFile.is_open( ) )
	{
		return;
	}

	String token;
	ISurface* pCurSuface = nullptr;
	Stringstream sStream;

	while ( materialFile.good( ) )
	{
		sStream.str( _T( "" ) );
		sStream.clear( );

		std::getline( materialFile, token );

		std::vector<String> params;
		UTIL::Split( token, params, _T( ' ' ) );

		UINT count = params.size( );

		if ( token.compare( 0, 1, _T( "#" ) ) == 0 )
		{
			//Do Nothing
		}
		else if ( count > 1 && token.find( _T( "newmtl" ) ) != String::npos )
		{
			std::unique_ptr<ISurface> newSurface = std::make_unique<CSurface>( );
			pCurSuface = pSurfaceManager->RegisterSurface( params[1], newSurface );
		}
		else if ( count > 1 && token.find( _T( "map_Kd" ) ) != String::npos )
		{
			if ( pCurSuface )
			{
				pCurSuface->SetTextureName( params[1] );
			}
		}
		else if ( count > 3 && token.find( _T( "Ka" ) ) != String::npos )
		{
			if ( pCurSuface )
			{
				D3DXCOLOR color;
				sStream << params[1] << ' ' <<
					params[2] << ' ' <<
					params[3];
				sStream >> color.r >> color.g >> color.b;
				color.a = 1;

				pCurSuface->SetAmbient( color );
			}
		}
		else if ( count > 3 && token.find( _T( "Kd" ) ) != String::npos )
		{
			if ( pCurSuface )
			{
				D3DXCOLOR color;
				sStream << params[1] << ' ' <<
					params[2] << ' ' <<
					params[3];
				sStream >> color.r >> color.g >> color.b;
				color.a = 1;

				pCurSuface->SetDiffuse( color );
			}
		}
		else if ( count > 3 && token.find( _T( "Ks" ) ) != String::npos )
		{
			if ( pCurSuface )
			{
				D3DXCOLOR color;
				sStream << params[1] << ' ' <<
					params[2] << ' ' <<
					params[3];
				sStream >> color.r >> color.g >> color.b;
				color.a = 1;

				pCurSuface->SetSpecular( color );
			}
		}
		else if ( count > 1 && token.find( _T( "Ns" ) ) != String::npos )
		{
			if ( pCurSuface )
			{
				sStream << params[1];

				float specularPower = 0.f;
				sStream >> specularPower;

				pCurSuface->SetSpeculaPower( specularPower );
			}
		}
	}

	materialFile.close( );
}