#include "stdafx.h"
#include "Model/ObjMeshLoader.h"

#include "Core/InterfaceFactories.h"
#include "FileSystem/EngineFileSystem.h"
#include "Math/CXMFloat.h"
#include "Model/CommonMeshDefine.h"
#include "Model/MeshDescription.h"
#include "Model/StaticMesh.h"
#include "Util.h"

#include <algorithm>
#include <assert.h>
#include <fstream>
#include <iostream>
#include <map>

using namespace DirectX;

ModelLoaderSharedHandle CObjMeshLoader::RequestAsyncLoad( const char* pFilePath, LoadCompletionCallback completionCallback )
{
	IFileSystem* fileSystem = GetInterface<IFileSystem>( );

	FileHandle objAsset = fileSystem->OpenFile( pFilePath );
	if ( objAsset.IsValid( ) == false )
	{
		return CreateHandle( );
	}

	unsigned long fileSize = fileSystem->GetFileSize( objAsset );
	if ( fileSize == 0 )
	{
		return CreateHandle( );
	}

	++m_lastContextID;
	auto result = m_contexts.emplace( m_lastContextID, ObjMeshLoadContext( ) );
	assert( result.second );

	ObjMeshLoadContext* pContext = &result.first->second;
	pContext->m_contextID = m_lastContextID;
	pContext->m_meshDirectory = UTIL::GetFilePath( pFilePath );
	pContext->m_handle = CreateHandle( );

	char* buffer = new char[fileSize];
	IFileSystem::IOCompletionCallback ParseObjAsset;
	ParseObjAsset.BindFunctor( 
		[this, pContext, objAsset]( char* buffer, unsigned long bufferSize )
		{
			ParseObjMesh( *pContext, buffer, bufferSize );
			delete[] buffer;
			GetInterface<IFileSystem>( )->CloseFile( objAsset );

			OnLoadSuccessed( *pContext );
		} );

	if ( fileSystem->ReadAsync( objAsset, buffer, fileSize, &ParseObjAsset ) == false )
	{
		delete[] buffer;
		OnLoadFailed( *pContext );
	}

	pContext->m_handle->BindCompletionCallback( completionCallback );
	pContext->m_handle->OnStartLoading( );

	return pContext->m_handle;
/*
#ifdef TEST_CODE
	for ( auto iter = m_vertices.begin( ); iter != m_vertices.end( ); ++iter )
	{
		DebugMsg( "v: %f, %f, %f\n", iter->x, iter->y, iter->z );
	}

	for ( auto iter = normals.begin( ); iter != normals.end( ); ++iter )
	{
		DebugMsg( "n: %f, %f, %f\n", iter->x, iter->y, iter->z );
	}
#endif

	if ( normals.size() == 0 )
	{
		CalcObjNormal();
		
		for ( auto& face : faceInfo )
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

	for ( const auto& mtl : mtlGroup )
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

	return nullptr;
	*/
}

void CObjMeshLoader::LoadMaterialFromFile( ObjMeshLoadContext& context )
{
	const char* pFilePath = context.m_mtlFileNames[context.m_loadingMaterialIndex].c_str( );
	IFileSystem* fileSystem = GetInterface<IFileSystem>( );

	FileHandle mtlAsset = fileSystem->OpenFile( pFilePath );
	if ( mtlAsset.IsValid( ) == false )
	{
		return;
	}

	unsigned long fileSize = fileSystem->GetFileSize( mtlAsset );
	if ( fileSize == 0 )
	{
		return;
	}

	char* buffer = new char[fileSize];
	IFileSystem::IOCompletionCallback ParseMtlAsset;
	ParseMtlAsset.BindFunctor(
		[this, &context, mtlAsset]( char* buffer, unsigned long bufferSize )
		{
			ParseMtl( context, buffer, bufferSize );
			delete[] buffer;
			GetInterface<IFileSystem>( )->CloseFile( mtlAsset );

			OnLoadSuccessed( context );
		} );

	if ( fileSystem->ReadAsync( mtlAsset, buffer, fileSize, &ParseMtlAsset ) == false )
	{
		delete[] buffer;
		OnLoadFailed( context );
	}
}

void CObjMeshLoader::ParseMtl( ObjMeshLoadContext& context, char* buffer, unsigned long bufferSize )
{
	assert( buffer );

	UTIL::ReplaceChar( buffer, buffer, '\r', ' ', static_cast<size_t>( bufferSize ), static_cast<size_t>( bufferSize ) );

	std::string token;
	Material* pCurMaterial = nullptr;
	std::stringstream materialFile( buffer );
	std::stringstream sStream;

	while ( materialFile.good( ) )
	{
		sStream.str( "" );
		sStream.clear( );

		std::getline( materialFile, token );

		std::vector<std::string> params;
		UTIL::Split( token, params, _T( ' ' ) );

		assert( params.size( ) <= UINT_MAX );
		UINT count = static_cast<UINT>( params.size( ) );

		if ( token.compare( 0, 1, "#" ) == 0 )
		{
			//Do Nothing
		}
		else if ( count > 1 && token.find( "newmtl" ) != std::string::npos )
		{
			context.m_meshMaterials.emplace_back( params[1].c_str( ) );
			pCurMaterial = &context.m_meshMaterials.back( );
		}
		else if ( count > 1 && token.find( "map_Kd" ) != std::string::npos )
		{
			assert( pCurMaterial != nullptr );
			pCurMaterial->AddProperty( "$DiffuseTexture", params[1].c_str( ) );
		}
		else if ( count > 3 && token.find( "Ka" ) != std::string::npos )
		{
			assert( pCurMaterial != nullptr );
			CXMFLOAT4 color;
			sStream << params[1] << ' ' <<
				params[2] << ' ' <<
				params[3];
			sStream >> color.x >> color.y >> color.z;
			color.w = 1;

			pCurMaterial->AddProperty( "$Ambient", color );

			assert( pCurMaterial->AsVector( "&Ambient" ) == color );
		}
		else if ( count > 3 && token.find( "Kd" ) != std::string::npos )
		{
			assert( pCurMaterial != nullptr );
			CXMFLOAT4 color;
			sStream << params[1] << ' ' <<
				params[2] << ' ' <<
				params[3];
			sStream >> color.x >> color.y >> color.z;
			color.w = 1;

			pCurMaterial->AddProperty( "$Diffuse", color );
		}
		else if ( count > 3 && token.find( "Ks" ) != std::string::npos )
		{
			assert( pCurMaterial != nullptr );
			CXMFLOAT4 color;
			sStream << params[1] << ' ' <<
				params[2] << ' ' <<
				params[3];
			sStream >> color.x >> color.y >> color.z;
			color.w = 1;

			pCurMaterial->AddProperty( "$Specular", color );
		}
		else if ( count > 1 && token.find( "Ns" ) != std::string::npos )
		{
			assert( pCurMaterial != nullptr );
			sStream << params[1];

			float specularPower = 0.f;
			sStream >> specularPower;

			pCurMaterial->AddProperty( "$SpecularPower", specularPower );
		}
	}
}

void CObjMeshLoader::ParseObjMesh( ObjMeshLoadContext& context, char* buffer, unsigned long bufferSize )
{
	assert( buffer != nullptr );

	UTIL::ReplaceChar( buffer, buffer, '\r', ' ', static_cast<size_t>( bufferSize ), static_cast<size_t>( bufferSize ) );

	MeshDescription& outMesh = context.m_meshDescription;

	std::string token;
	std::stringstream meshFile( buffer );
	std::stringstream sStream;

	std::map<MeshVertexInstance, std::size_t> viLut;

	while ( meshFile.good( ) )
	{
		sStream.str( "" );
		sStream.clear( );

		std::getline( meshFile, token );

		std::vector<std::string> params;
		UTIL::Split( token, params, ' ' );

		UINT count = static_cast<UINT>( params.size( ) );

		if ( token.compare( 0, 1, "#" ) == 0 )
		{
			//Do Nothing
		}
		else if ( token.find( "vn" ) != std::string::npos )
		{
			assert( count == 4 );

			sStream << params[1].c_str( ) << ' ' <<
				params[2].c_str( ) << ' ' <<
				params[3].c_str( );

			float x = 0.f, y = 0.f, z = 0.f;
			sStream >> x >> y >> z;

			outMesh.m_normals.emplace_back( x, y, z );
		}
		else if ( token.find( "vt" ) != std::string::npos )
		{
			assert( count == 3 );

			sStream << params[1].c_str( ) << ' ' <<
				params[2].c_str( );

			float u = 0.f, v = 0.f;
			sStream >> u >> v;

			outMesh.m_texCoords.emplace_back( u, 1.0f - v );
		}
		else if ( token.find( "v " ) != std::string::npos )
		{
			assert( count == 4 );

			sStream << params[1].c_str( ) << ' ' <<
				params[2].c_str( ) << ' ' <<
				params[3].c_str( );

			float x = 0.f, y = 0.f, z = 0.f;
			sStream >> x >> y >> z;

			assert( !sStream.fail( ) && !sStream.bad( ) && sStream.eof( ) );

			outMesh.m_positions.emplace_back( x, y, z );
		}
		else if ( token.find( "f " ) != std::string::npos )
		{
			assert( count == 4 );

			outMesh.m_triangles.emplace_back( );
			MeshTriangle& triangle = outMesh.m_triangles.back( );

			for ( UINT i = 1; i < count; ++i )
			{
				std::vector<std::string> face;

				UTIL::Split( params[i], face, _T( '/' ) );
				int pos = -1;
				int tex = -1;
				int normal = -1;

				for ( std::size_t j = 0; j < face.size(); ++j )
				{
					int idx = atoi( face[j].c_str( ) ) - 1;

					if ( idx >= 0 )
					{
						switch ( j )
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

				std::size_t vertexInstanceID = 0;
				MeshVertexInstance vertexInstance( pos, tex, normal );
				auto found = viLut.find( vertexInstance );
				if ( found == viLut.end( ) )
				{
					outMesh.m_vertexInstances.push_back( vertexInstance );
					vertexInstanceID = outMesh.m_vertexInstances.size( ) - 1;
					viLut.emplace( vertexInstance, vertexInstanceID );
				}
				else
				{
					vertexInstanceID = found->second;
				}

				triangle.m_vertexInstanceID[i - 1] = vertexInstanceID;
			}

			if ( outMesh.m_polygons.size( ) == 0 )
			{
				outMesh.m_polygons.emplace_back( );
			}

			outMesh.m_polygons.back( ).m_triangleID.push_back( outMesh.m_triangles.size( ) - 1 );
		}
		else if ( token.find( "mtllib" ) != std::string::npos )
		{
			if ( count > 1 )
			{
				context.m_mtlFileNames.emplace_back( context.m_meshDirectory + params[1] );
			}
		}
		else if ( token.find( "usemtl" ) != std::string::npos )
		{
			if ( count > 1 )
			{
				outMesh.m_polygonMaterialName.emplace_back( std::move( params[1] ) );
				outMesh.m_polygons.emplace_back( );
			}
		}
	}
}

//std::vector<MeshVertex> CObjMeshLoader::BuildVertices( )
//{
//	std::vector<MeshVertex> vertices;
//
//	MeshVertex curVertex;
//	CXMFLOAT3 randColor( static_cast<float>( rand( ) ) / RAND_MAX,
//		static_cast<float>( rand( ) ) / RAND_MAX,
//		static_cast<float>( rand( ) ) / RAND_MAX );
//
//	assert( faceInfo.size( ) <= UINT_MAX );
//	UINT count = static_cast<UINT>( faceInfo.size( ) );
//
//	UINT startGroupIdx = 0;
//	auto iter = faceMtlGroup.begin( );
//
//	for ( UINT i = 0; i < count; ++i )
//	{
//		::ZeroMemory( &curVertex, VERTEX_STRIDE );
//
//		const ObjFaceInfo &info = faceInfo[i];
//
//		if ( info.m_position != -1 )
//		{
//			curVertex.m_position = positions[info.m_position];
//		}
//
//		if ( info.m_normal != -1 )
//		{
//			curVertex.m_normal = normals[info.m_normal];
//		}
//
//		if ( info.m_texCoord != -1 )
//		{
//			curVertex.m_texcoord = texCoords[info.m_texCoord];
//		}
//
//		curVertex.m_color = randColor;
//
//		vertices.push_back( curVertex );
//
//		if ( iter != faceMtlGroup.end() && i == iter->m_endFaceIndex )
//		{
//			mtlGroup.emplace_back( startGroupIdx, i, iter->m_materialName );
//			startGroupIdx = i + 1;
//			++iter;
//		}
//	}
//
//	return vertices;
//}

//void CObjMeshLoader::CalcObjNormal( )
//{
//	normals.resize( positions.size() );
//
//	for ( CXMFLOAT3& normal : normals )
//	{
//		normal.x = 0.f;
//		normal.y = 0.f;
//		normal.z = 0.f;
//	}
//
//	std::vector<UINT> idxList;
//	idxList.reserve( std::max( positions.size( ), faceInfo.size() ) );
//
//	if ( faceInfo.size() != 0 )
//	{
//		for ( auto face : faceInfo )
//		{
//			idxList.push_back( face.m_position );
//		}
//	}
//	else
//	{
//		for ( UINT i = 0; i < positions.size( ); ++i )
//		{
//			idxList.push_back( i );
//		}
//	}
//
//	for ( std::size_t i = 0; i < idxList.size( ); i += 3 )
//	{
//		const CXMFLOAT3& p0 = positions[idxList[i]];
//		const CXMFLOAT3& p1 = positions[idxList[i + 1]];
//		const CXMFLOAT3& p2 = positions[idxList[i + 2]];
//
//		const CXMFLOAT3& v0 = p1 - p0;
//		const CXMFLOAT3& v1 = p2 - p0;
//
//		CXMFLOAT3 normal = XMVector3Cross( v0, v1 );
//
//		normals[idxList[i]] += normal;
//		normals[idxList[i + 1]] += normal;
//		normals[idxList[i + 2]] += normal;
//	}
//
//	for ( CXMFLOAT3& normal : normals )
//	{
//		normal = XMVector3Normalize( normal );
//	}
//}

void CObjMeshLoader::OnLoadSuccessed( ObjMeshLoadContext& context )
{
	bool needMaterialLoad = ( context.m_loadingMaterialIndex < context.m_mtlFileNames.size( ) );

	if ( needMaterialLoad )
	{
		LoadMaterialFromFile( context );
		++context.m_loadingMaterialIndex;
	}
	else
	{
		// Currently only support static mesh
		void* asset = new StaticMesh( std::move( context.m_meshDescription ), std::move( context.m_meshMaterials ) );

		// Handle completion callback
		context.m_handle->SetLoadedAsset( asset );
		context.m_handle->ExecuteCompletionCallback( );
		m_contexts.erase( context.m_contextID );
	}
}

void CObjMeshLoader::OnLoadFailed( ObjMeshLoadContext& context )
{
	m_contexts.erase( context.m_contextID );
}

std::shared_ptr<ModelLoaderHandle> CObjMeshLoader::CreateHandle( )
{
	return std::make_shared<ModelLoaderHandle>( );
}
