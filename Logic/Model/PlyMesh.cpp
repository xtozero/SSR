#include "stdafx.h"
#include "PlyMesh.h"

#include "../GameLogic.h"
#include "../../RenderCore/CommonRenderer/IMaterial.h"
#include "../../RenderCore/CommonRenderer/IRenderer.h"

void CPlyMesh::Draw( CGameLogic& gameLogic )
{
	if ( !m_pMaterial )
	{
		return;
	}

	if ( m_vertexBuffer == RE_HANDLE_TYPE::INVALID_HANDLE )
	{
		return;
	}

	IRenderer& renderer = gameLogic.GetRenderer( );

	m_pMaterial->Bind( renderer );
	renderer.BindVertexBuffer( &m_vertexBuffer, 0, 1, &m_stride, &m_offset );
	if ( m_indexBuffer == RE_HANDLE_TYPE::INVALID_HANDLE )
	{
		renderer.Draw( m_primitiveTopology, m_nVertices, m_offset );
	}
	else
	{
		renderer.BindIndexBuffer16( m_indexBuffer, m_nIndexOffset );
		renderer.DrawIndexed( m_primitiveTopology, m_nIndices, m_nIndexOffset, m_offset );
	}
}
