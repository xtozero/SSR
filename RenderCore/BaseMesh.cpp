#include "stdafx.h"
#include "BaseMesh.h"
#include "common.h"
#include "CommonMeshDefine.h"
#include "IMaterial.h"
#include "IRenderer.h"

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

bool BaseMesh::Load( IRenderer& renderer, D3D_PRIMITIVE_TOPOLOGY topology )
{
	m_primitiveTopology = topology;

	UINT stride = VERTEX_STRIDE;

	m_pVertexBuffer = renderer.CreateVertexBuffer( stride, m_nVertices, m_pModelData );

	if ( !m_pVertexBuffer )
	{
		return false;
	}

	if ( m_pIndexData )
	{
		m_pIndexBuffer = renderer.CreateIndexBuffer( sizeof( WORD ), m_nIndices, m_pIndexData );
	}

	return true;
}

void BaseMesh::Draw( ID3D11DeviceContext* )
{

}

void BaseMesh::SetMaterial( IMaterial* pMaterial )
{
	m_pMaterial = pMaterial;
}

BaseMesh::BaseMesh( ) : m_pModelData( nullptr ),
m_pIndexData( nullptr ),
m_pVertexBuffer( nullptr ),
m_pIndexBuffer( nullptr ),
m_primitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED ),
m_pMaterial( nullptr ),
m_nVertices( 0 ),
m_nOffset( 0 ),
m_nIndices( 0 ),
m_nIndexOffset( 0 )
{
}

BaseMesh::~BaseMesh( )
{
	SAFE_ARRAY_DELETE( m_pModelData );
	SAFE_ARRAY_DELETE( m_pIndexData );
}