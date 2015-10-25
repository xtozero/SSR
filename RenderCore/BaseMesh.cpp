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

bool BaseMesh::Load( )
{
	return true;
}

bool BaseMesh::LoadMaterial( const TCHAR* pMaterialName )
{
	m_pMaterial = MaterialSystem::GetInstance()->SearchMaterialByName( pMaterialName );

	return m_pMaterial ? true : false;
}

BaseMesh::BaseMesh( ) : m_pModelData( NULL ),
m_pVertexBuffer( NULL ),
m_pIndexBuffer( NULL ),
m_primitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED ),
m_pMaterial( NULL ),
m_nVertices( 0 ),
m_nOffset( 0 )
{
}

BaseMesh::~BaseMesh( )
{
	//Do Not Delete Bufferes
	SAFE_ARRAY_DELETE( m_pModelData );
}