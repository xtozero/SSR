#include "stdafx.h"
#include "Model/BaseMesh.h"

#include "common.h"
#include "Model/CommonMeshDefine.h"
//#include "Render/IRenderer.h"

void BaseMesh::SetName( const char* name )
{
	strncpy_s( m_name, name, MAX_MESH_NAME );
}

void BaseMesh::SetModelData( Owner<MeshVertex*> pOrignal, UINT vertexCount )
{
	m_pModelData = pOrignal;
	m_nVertices = vertexCount;
}

void BaseMesh::SetIndexData( Owner<void*> pOrignal, UINT indexCount )
{
	m_pIndexData = pOrignal;
	m_nIndices = indexCount;
}

void BaseMesh::SetColor( const CXMFLOAT3& color )
{
	m_color = color;
}

//bool BaseMesh::Load( IRenderer& renderer, UINT primitive )
//{
//	m_primitiveTopology = primitive;
//
//	m_stride = VERTEX_STRIDE;
//
//	BUFFER_TRAIT trait = { m_stride * m_nVertices,
//							1,
//							RESOURCE_ACCESS_FLAG::GPU_READ,
//							RESOURCE_BIND_TYPE::VERTEX_BUFFER,
//							0,
//							m_pModelData,
//							0,
//							0 };
//
//	m_vertexBuffer = renderer.CreateBuffer( trait );
//
//	if ( m_vertexBuffer == RE_HANDLE::InValidHandle( ) )
//	{
//		return false;
//	}
//
//	if ( m_pIndexData )
//	{
//		trait.m_stride = sizeof( WORD );
//		trait.m_count = m_nIndices;
//		trait.m_bindType = RESOURCE_BIND_TYPE::INDEX_BUFFER;
//		trait.m_srcData = m_pIndexData;
//
//		m_indexBuffer = renderer.CreateBuffer( trait );
//	}
//
//	return true;
//}
//
//void BaseMesh::SetMaterial( Material material )
//{
//	m_material = material;
//}

BaseMesh::~BaseMesh( )
{
	delete[] m_pModelData;
	delete[] m_pIndexData;
}