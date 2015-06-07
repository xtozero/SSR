#include "stdafx.h"
#include "common.h"
#include "DebugMesh.h"
#include <D3Dx10.h>
#include "Direct3D11.h"

extern IRenderer* g_pRenderer;

bool TriangleMesh::Load( )
{
	m_pModelData = new D3DXVECTOR3[3];

	D3DXVECTOR3* vertex = static_cast<D3DXVECTOR3*>( m_pModelData );

	vertex[0] = D3DXVECTOR3( 0.0f, 0.5f, 0.0f );
	vertex[1] = D3DXVECTOR3( 0.0f, 0.5f, 0.0f );
	vertex[2] = D3DXVECTOR3( 0.0f, 0.5f, 0.0f );

	UINT stride = sizeof( D3DXVECTOR3 );
	UINT numOfElement = 3;

	m_pVertexBuffer = g_pRenderer->CreateVertexBuffer( stride, numOfElement, m_pModelData );

	if ( !m_pVertexBuffer )
	{
		return false;
	}

	return true;
}

TriangleMesh::TriangleMesh( )
{
}
