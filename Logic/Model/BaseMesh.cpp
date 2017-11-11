#include "stdafx.h"
#include "BaseMesh.h"
#include "../common.h"
#include "CommonMeshDefine.h"
#include "../../RenderCore/CommonRenderer/IMaterial.h"
#include "../../RenderCore/CommonRenderer/IRenderer.h"

void BaseMesh::SetName( const TCHAR* name )
{
	_tcsncpy_s( m_name, name, MAX_MESH_NAME );
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

bool BaseMesh::Load( IRenderer& renderer, UINT primitive )
{
	m_primitiveTopology = primitive;

	m_stride = VERTEX_STRIDE;

	BUFFER_TRAIT trait = { m_stride * m_nVertices,
							1,
							RESOURCE_ACCESS_FLAG::GPU_READ,
							RESOURCE_TYPE::VERTEX_BUFFER,
							0,
							m_pModelData,
							0,
							0 };

	m_pVertexBuffer = renderer.CreateBuffer( trait );

	if ( !m_pVertexBuffer )
	{
		return false;
	}

	if ( m_pIndexData )
	{
		trait.m_stride = sizeof( WORD );
		trait.m_count = m_nIndices;
		trait.m_bufferType = RESOURCE_TYPE::INDEX_BUFFER;
		trait.m_srcData = m_pIndexData;

		m_pIndexBuffer = renderer.CreateBuffer( trait );
	}

	return true;
}

void BaseMesh::Draw( CGameLogic& )
{
}

void BaseMesh::SetMaterial( IMaterial* pMaterial )
{
	m_pMaterial = pMaterial;
}

BaseMesh::~BaseMesh( )
{
	delete[] m_pModelData;
	delete[] m_pIndexData;
}