#include "stdafx.h"
#include "common.h"
#include "DebugMesh.h"
#include "IRenderer.h"

#include "Material.h"

#include "../shared/Math/CXMFloat.h"

bool TriangleMesh::Load( IRenderer& renderer, D3D_PRIMITIVE_TOPOLOGY topology )
{
	m_primitiveTopology = topology;

	m_pModelData = new CXMFLOAT3[3];

	CXMFLOAT3* vertex = static_cast<CXMFLOAT3*>( m_pModelData );

// 	vertex[0] = D3DXVECTOR3( 0.0f, 0.5f, 0.0f );
// 	vertex[1] = D3DXVECTOR3( 0.5f, -0.5f, 0.0f );
// 	vertex[2] = D3DXVECTOR3( -0.5f, -0.5f, 0.0f );

	vertex[0] = CXMFLOAT3( 0.0f, 200.f, 500.0f );
	vertex[1] = CXMFLOAT3( 200.f, -200.f, 500.0f );
	vertex[2] = CXMFLOAT3( -200.f, -200.f, 500.0f );

	m_stride = sizeof( CXMFLOAT3 );
	m_nVertices = 3;

	BUFFER_TRAIT trait = { m_stride,
							m_nVertices,
							BUFFER_ACCESS_FLAG::GPU_READ,
							BUFFER_TYPE::VERTEX_BUFFER,
							0,
							m_pModelData,
							0,
							0 };

	m_pVertexBuffer = renderer.CreateBuffer( trait );

	if ( !m_pVertexBuffer )
	{
		return false;
	}

	SetMaterial( renderer.GetMaterialPtr( _T( "tutorial" ) ) );
	
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

	m_pVertexBuffer->SetVertexBuffer( &m_stride, &m_nOffset );
	m_pMaterial->SetShader( pDeviceContext );
	m_pMaterial->SetPrimitiveTopology( pDeviceContext, m_primitiveTopology );
	m_pMaterial->Draw( pDeviceContext, m_nVertices, m_nOffset);
}

TriangleMesh::TriangleMesh( )
{
}
