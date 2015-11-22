#include "stdafx.h"
#include "BaseMesh.h"
#include "common.h"
#include <D3Dx10.h>
#include "Direct3D11.h"

#include "MaterialSystem.h"

extern IRenderer* g_pRenderer;

void BaseMesh::SetModelData( void* pOrignal, UINT vertexCount )
{
	m_pModelData = pOrignal;
	m_nVertices = vertexCount;
}

void BaseMesh::SetIndexData( void* pOrignal, UINT indexCount )
{
	m_pIndexData = pOrignal;
	m_nIndices = indexCount;
}

bool BaseMesh::Load( )
{
	return true;
}

void BaseMesh::Draw( ID3D11DeviceContext* pDeviceContext )
{

}

bool BaseMesh::LoadMaterial( const TCHAR* pMaterialName )
{
	m_pMaterial = MaterialSystem::GetInstance()->SearchMaterialByName( pMaterialName );

	return m_pMaterial ? true : false;
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