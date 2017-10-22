#include "stdafx.h"
#include "DebugMesh.h"

#include "../common.h"
#include "../GameLogic.h"
#include "../../RenderCore/CommonRenderer/IMaterial.h"
#include "../../RenderCore/CommonRenderer/IRenderer.h"

#include "../../shared/Math/CXMFloat.h"

bool TriangleMesh::Load( IRenderer& renderer, UINT primitive )
{
	m_primitiveTopology = primitive;

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

	SetMaterial( renderer.GetMaterialPtr( _T( "tutorial" ) ) );
	
	return true;
}

void TriangleMesh::Draw( CGameLogic& gameLogic )
{
	if ( !m_pMaterial )
	{
		return;
	}

	if ( !m_pVertexBuffer )
	{
		return;
	}

	m_pVertexBuffer->SetVertexBuffer( &m_stride, &m_nOffset );
	m_pMaterial->SetShader( );
	gameLogic.GetRenderer().Draw( m_primitiveTopology, m_nVertices, m_nOffset);
}

TriangleMesh::TriangleMesh( )
{
}
