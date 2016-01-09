#include "stdafx.h"
#include "BaseMesh.h"
#include "common.h"
#include "CommonMeshDefine.h"
#include <D3Dx10.h>
#include "Direct3D11.h"

#include "MaterialSystem.h"

extern IRenderer* g_pRenderer;

void BaseMesh::SetModelData( MeshVertex* pOrignal, UINT vertexCount )
{
	m_pModelData = pOrignal;
	m_nVertices = vertexCount;
}

void BaseMesh::SetIndexData( void* pOrignal, UINT indexCount )
{
	m_pIndexData = pOrignal;
	m_nIndices = indexCount;
}

void BaseMesh::SetColor( const D3DXVECTOR3& color )
{
	m_color = color;
}

bool BaseMesh::Load( )
{
	m_primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	UINT stride = VERTEX_STRIDE;

	m_pVertexBuffer = g_pRenderer->CreateVertexBuffer( stride, m_nVertices, m_pModelData );

	if ( !m_pVertexBuffer )
	{
		return false;
	}

	if ( m_pIndexData )
	{
		m_pIndexBuffer = g_pRenderer->CreateIndexBuffer( sizeof( WORD ), m_nIndices, m_pIndexData );
	}

	return true;
}

void BaseMesh::Draw( ID3D11DeviceContext* )
{

}

void BaseMesh::SetMaterial( const std::shared_ptr<IMaterial> pMaterial )
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
	//Do Not Delete Bufferes
	SAFE_ARRAY_DELETE( m_pModelData );
	SAFE_ARRAY_DELETE( m_pIndexData );
}