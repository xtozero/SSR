#include "stdafx.h"
#include "Model/ObjMeshLoader.h"

#include "Math/CXMFloat.h"
#include "Model/CommonMeshDefine.h"
#include "Model/ObjMesh.h"
#include "Util.h"

#include <algorithm>
#include <assert.h>
#include <fstream>
#include <iostream>

using namespace DirectX;

namespace
{
	enum OBJ_FACE_ELEMENT
	{
		VERTEX = 0,
		TEXTURE,
		NORMAL,
	};

	constexpr TCHAR* OBJ_FILE_DIR = _T( "../model/obj/" );
}

Owner<IMesh*> CObjMeshLoader::LoadMeshFromFile( IRenderer& renderer, const TCHAR* pFileName, SurfaceMap& surface )
{
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

				for ( auto iter = face.begin( ); iter != face.end( ); ++iter )
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
				assert( m_faceInfo.size( ) <= UINT_MAX );
				iter->m_endFaceIndex = std::max( static_cast<UINT>( m_faceInfo.size( ) - 1 ), 0U );
			}
		}
		else if ( token.find( _T( "mtllib" ) ) != String::npos )
		{
			if ( count > 1 )
			{
				LoadMaterialFile( params[1].c_str(), surface );
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

	if ( m_normals.size() == 0 )
	{
		CalcObjNormal();
		
		for ( auto& face : m_faceInfo )
		{
			face.m_normal = face.m_position;
		}
	}

	const std::vector<MeshVertex>& buildedVertices = BuildVertices( );
	std::vector<WORD> buildedIndices;
	
	for ( auto iter = buildedVertices.begin(); iter != buildedVertices.end(); ++iter )
	{
		buildedIndices.push_back( static_cast<WORD>( std::distance( buildedVertices.begin( ), iter ) ) );
	}

	assert( buildedVertices.size( ) <= UINT_MAX );
	UINT vertexCount = static_cast<UINT>( buildedVertices.size( ) );
	assert( buildedIndices.size( ) <= UINT_MAX );
	UINT indexCount = static_cast<UINT>( buildedIndices.size( ) );

	MeshVertex* vertices = new MeshVertex[vertexCount];
	WORD* indices = new WORD[indexCount];

	::memcpy_s( vertices, sizeof(MeshVertex) * vertexCount, &buildedVertices[0], sizeof(MeshVertex) * vertexCount );
	::memcpy_s( indices, sizeof(WORD) * indexCount, &buildedIndices[0], sizeof(WORD) * indexCount );

	CObjMesh* newMesh = new CObjMesh;

	for ( const auto& mtl : m_mtlGroup )
	{
		ObjSurfaceTrait trait;

		trait.m_indexOffset = mtl.m_startIndex;
		trait.m_indexCount = mtl.m_endIndex - mtl.m_startIndex + 1;

		Surface* found = FindSurface( surface, mtl.m_materialName );
		if ( found == nullptr )
		{
			__debugbreak( );
		}

		trait.m_pSurface = found;

		newMesh->AddMaterialGroup( trait );
	}
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
	CXMFLOAT3 randColor( static_cast<float>( rand( ) ) / RAND_MAX,
		static_cast<float>( rand( ) ) / RAND_MAX,
		static_cast<float>( rand( ) ) / RAND_MAX );

	assert( m_faceInfo.size( ) <= UINT_MAX );
	UINT count = static_cast<UINT>( m_faceInfo.size( ) );

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

void CObjMeshLoader::LoadMaterialFile( const TCHAR* pFileName, SurfaceMap& surface )
{
	Ifstream materialFile( pFileName, 0 );

	if ( !materialFile.is_open( ) )
	{
		return;
	}

	String token;
	Surface* pCurSuface = nullptr;
	Stringstream sStream;

	while ( materialFile.good( ) )
	{
		sStream.str( _T( "" ) );
		sStream.clear( );

		std::getline( materialFile, token );

		std::vector<String> params;
		UTIL::Split( token, params, _T( ' ' ) );

		assert( params.size( ) <= UINT_MAX );
		UINT count = static_cast<UINT>( params.size( ) );

		if ( token.compare( 0, 1, _T( "#" ) ) == 0 )
		{
			//Do Nothing
		}
		else if ( count > 1 && token.find( _T( "newmtl" ) ) != String::npos )
		{
			std::unique_ptr<Surface> newSurface = std::make_unique<Surface>( );
			pCurSuface = RegisterSurface( surface, params[1], std::move( newSurface ) );
		}
		else if ( count > 1 && token.find( _T( "map_Kd" ) ) != String::npos )
		{
			if ( pCurSuface )
			{
				pCurSuface->m_diffuseTexName = params[1];
			}
		}
		else if ( count > 3 && token.find( _T( "Ka" ) ) != String::npos )
		{
			if ( pCurSuface )
			{
				CXMFLOAT4 color;
				sStream << params[1] << ' ' <<
					params[2] << ' ' <<
					params[3];
				sStream >> color.x >> color.y >> color.z;
				color.w = 1;

				pCurSuface->m_trait.m_ambient = color;
			}
		}
		else if ( count > 3 && token.find( _T( "Kd" ) ) != String::npos )
		{
			if ( pCurSuface )
			{
				CXMFLOAT4 color;
				sStream << params[1] << ' ' <<
					params[2] << ' ' <<
					params[3];
				sStream >> color.x >> color.y >> color.z;
				color.w = 1;

				pCurSuface->m_trait.m_diffuse = color;
			}
		}
		else if ( count > 3 && token.find( _T( "Ks" ) ) != String::npos )
		{
			if ( pCurSuface )
			{
				CXMFLOAT4 color;
				sStream << params[1] << ' ' <<
					params[2] << ' ' <<
					params[3];
				sStream >> color.x >> color.y >> color.z;
				color.w = 1;

				pCurSuface->m_trait.m_specular = color;
			}
		}
		else if ( count > 1 && token.find( _T( "Ns" ) ) != String::npos )
		{
			if ( pCurSuface )
			{
				sStream << params[1];

				float specularPower = 0.f;
				sStream >> specularPower;

				pCurSuface->m_trait.m_specularPower = specularPower;
			}
		}
	}

	materialFile.close( );
}

void CObjMeshLoader::CalcObjNormal( )
{
	m_normals.resize( m_positions.size() );

	for ( CXMFLOAT3& normal : m_normals )
	{
		normal.x = 0.f;
		normal.y = 0.f;
		normal.z = 0.f;
	}

	std::vector<UINT> idxList;
	idxList.reserve( std::max( m_positions.size( ), m_faceInfo.size() ) );

	if ( m_faceInfo.size() != 0 )
	{
		for ( auto face : m_faceInfo )
		{
			idxList.push_back( face.m_position );
		}
	}
	else
	{
		for ( UINT i = 0; i < m_positions.size( ); ++i )
		{
			idxList.push_back( i );
		}
	}

	for ( std::size_t i = 0; i < idxList.size( ); i += 3 )
	{
		const CXMFLOAT3& p0 = m_positions[idxList[i]];
		const CXMFLOAT3& p1 = m_positions[idxList[i + 1]];
		const CXMFLOAT3& p2 = m_positions[idxList[i + 2]];

		const CXMFLOAT3& v0 = p1 - p0;
		const CXMFLOAT3& v1 = p2 - p0;

		CXMFLOAT3 normal = XMVector3Cross( v0, v1 );

		m_normals[idxList[i]] += normal;
		m_normals[idxList[i + 1]] += normal;
		m_normals[idxList[i + 2]] += normal;
	}

	for ( CXMFLOAT3& normal : m_normals )
	{
		normal = XMVector3Normalize( normal );
	}
}
