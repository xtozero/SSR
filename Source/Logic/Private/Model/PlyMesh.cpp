#include "stdafx.h"
#include "Model/PlyMesh.h"

#include "Core/GameLogic.h"
//#include "Render/IRenderer.h"

//void CPlyMesh::Draw( CGameLogic& gameLogic )
//{
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
	//if ( m_indexBuffer == RE_HANDLE::InValidHandle( ) )
	//{
	//	renderer.Draw( m_primitiveTopology, m_nVertices, m_offset );
	//}
	//else
	//{
	//	renderer.BindIndexBuffer( m_indexBuffer, m_nIndexOffset );
	//	renderer.DrawIndexed( m_primitiveTopology, m_nIndices, m_nIndexOffset, m_offset );
	//}
//}
