#include "stdafx.h"
#include "PlyMesh.h"

#include "common.h"
#include "CommonMeshDefine.h"
#include "CommonRenderer/ConstantBufferDefine.h"

#include "CommonRenderer/ISurface.h"
#include "CommonRenderer/IRenderer.h"

#include "Material.h"

void CPlyMesh::Draw( IRenderer& renderer )
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
	if ( m_pSurface != nullptr )
	{
		m_pMaterial->SetSurface( SHADER_TYPE::PS, static_cast<UINT>(PS_CONSTANT_BUFFER::SURFACE), m_pSurface );
	}
	m_pVertexBuffer->SetVertexBuffer( &m_stride, &m_nOffset );
	if ( m_pIndexBuffer )
	{
		m_pIndexBuffer->SetIndexBuffer( sizeof( WORD ), m_nIndexOffset );
		renderer.DrawIndexed( m_primitiveTopology, m_nIndices, m_nIndexOffset, m_nOffset );
	}
	else
	{
		renderer.Draw( m_primitiveTopology, m_nVertices, m_nOffset );
	}
}

CPlyMesh::CPlyMesh( ) : m_pSurface( nullptr )
{
}