#include "stdafx.h"
#include "BaseMesh.h"
#include "common.h"

bool BaseMesh::Load( )
{
	return true;
}

BaseMesh::BaseMesh( ) : m_pModelData( NULL ),
m_pVertexBuffer( NULL ),
m_pIndexBuffer( NULL ),
m_primitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED )
{
}

BaseMesh::~BaseMesh( )
{
	//Do Not Delete Bufferes
	SAFE_ARRAY_DELETE( m_pModelData );
}
