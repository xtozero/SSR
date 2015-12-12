#include "stdafx.h"
#include "common.h"
#include "DebugMesh.h"
#include <D3Dx10.h>
#include "Direct3D11.h"

#include "Material.h"
#include "MaterialSystem.h"

extern IRenderer* g_pRenderer;

bool TriangleMesh::Load( )
{
	m_primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_pModelData = new D3DXVECTOR3[3];

	D3DXVECTOR3* vertex = static_cast<D3DXVECTOR3*>( m_pModelData );

// 	vertex[0] = D3DXVECTOR3( 0.0f, 0.5f, 0.0f );
// 	vertex[1] = D3DXVECTOR3( 0.5f, -0.5f, 0.0f );
// 	vertex[2] = D3DXVECTOR3( -0.5f, -0.5f, 0.0f );

	vertex[0] = D3DXVECTOR3( 0.0f, 200.f, 500.0f );
	vertex[1] = D3DXVECTOR3( 200.f, -200.f, 500.0f );
	vertex[2] = D3DXVECTOR3( -200.f, -200.f, 500.0f );

	UINT stride = sizeof( D3DXVECTOR3 );
	m_nVertices = 3;

	m_pVertexBuffer = g_pRenderer->CreateVertexBuffer( stride, m_nVertices, m_pModelData );

	if ( !m_pVertexBuffer )
	{
		return false;
	}

	SetMaterial( g_pRenderer->GetMaterialPtr( _T( "tutorial" ) ) );
	
	return true;
}

void TriangleMesh::Draw( ID3D11DeviceContext* pDeviceContext )
{
	if ( !pDeviceContext )
	{
		return;
	}

	if ( !m_pMaterial )
	{
		return;
	}

	if ( !m_pVertexBuffer )
	{
		return;
	}

	m_pVertexBuffer->SetIABuffer( pDeviceContext, &m_nOffset );
	m_pMaterial->SetShader( pDeviceContext );
	m_pMaterial->SetPrimitiveTopology( pDeviceContext, m_primitiveTopology );
	m_pMaterial->Draw( pDeviceContext, m_nVertices, m_nOffset);
}

TriangleMesh::TriangleMesh( )
{
}
