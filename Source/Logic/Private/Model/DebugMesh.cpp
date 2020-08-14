#include "stdafx.h"
#include "Model/DebugMesh.h"

#include "common.h"
#include "Core/GameLogic.h"
#include "Math/CXMFloat.h"
//#include "Render/IRenderer.h"

//bool TriangleMesh::Load( IRenderer& renderer, UINT primitive )
//{
//	m_primitiveTopology = primitive;
//
//	m_pModelData = new CXMFLOAT3[3];
//
//	CXMFLOAT3* vertex = static_cast<CXMFLOAT3*>( m_pModelData );
//
//// 	vertex[0] = D3DXVECTOR3( 0.0f, 0.5f, 0.0f );
//// 	vertex[1] = D3DXVECTOR3( 0.5f, -0.5f, 0.0f );
//// 	vertex[2] = D3DXVECTOR3( -0.5f, -0.5f, 0.0f );
//
//	vertex[0] = CXMFLOAT3( 0.0f, 200.f, 500.0f );
//	vertex[1] = CXMFLOAT3( 200.f, -200.f, 500.0f );
//	vertex[2] = CXMFLOAT3( -200.f, -200.f, 500.0f );
//
//	m_stride = sizeof( CXMFLOAT3 );
//	m_nVertices = 3;
//
//	BUFFER_TRAIT trait = { m_stride * m_nVertices,
//							1,
//							RESOURCE_ACCESS_FLAG::GPU_READ,
//							RESOURCE_BIND_TYPE::VERTEX_BUFFER,
//							0,
//							m_pModelData,
//							0,
//							0 };
//
//	m_vertexBuffer = renderer.CreateBuffer( trait );
//
//	if ( m_vertexBuffer == RE_HANDLE::InValidHandle( ) )
//	{
//		return false;
//	}
//
//	SetMaterial( renderer.SearchMaterial( _T( "tutorial" ) ) );
//	
//	return true;
//}

void TriangleMesh::Draw( CGameLogic& gameLogic )
{
	//if ( m_material == INVALID_MATERIAL )
	//{
	//	return;
	//}

	//if ( m_vertexBuffer == RE_HANDLE::InValidHandle( ) )
	//{
	//	return;
	//}

	//IRenderer& renderer = gameLogic.GetRenderer( );

	//renderer.BindMaterial( m_material );
	//renderer.BindVertexBuffer( &m_vertexBuffer, 0, 1, &m_stride, &m_offset );
	//gameLogic.GetRenderer().Draw( m_primitiveTopology, m_nVertices, m_offset);
}

TriangleMesh::TriangleMesh( )
{
}
