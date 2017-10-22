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

	if ( !m_pVertexBuffer )
	{
		return;
	}

	m_pMaterial->SetShader( );
	m_pVertexBuffer->SetVertexBuffer( &m_stride, &m_nOffset );
	if ( m_pIndexBuffer )
	{
		m_pIndexBuffer->SetIndexBuffer( sizeof( WORD ), m_nIndexOffset );
		gameLogic.GetRenderer().DrawIndexed( m_primitiveTopology, m_nIndices, m_nIndexOffset, m_nOffset );
	}
	else
	{
		gameLogic.GetRenderer().Draw( m_primitiveTopology, m_nVertices, m_nOffset );
	}
}
