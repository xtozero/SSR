#include "stdafx.h"
#include "common.h"
#include "CommonMeshDefine.h"
#include "ConstantBufferDefine.h"

#include "Material.h"

#include "PlyMesh.h"
#include "ISurface.h"

void CPlyMesh::Draw( ID3D11DeviceContext* pDeviceContext )
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

	m_pMaterial->SetShader( pDeviceContext );
	m_pMaterial->SetPrimitiveTopology( pDeviceContext, m_primitiveTopology );
	if ( m_pSurface != nullptr )
	{
		m_pMaterial->SetSurface( pDeviceContext, SHADER_TYPE::PS, static_cast<UINT>(PS_CONSTANT_BUFFER::SURFACE), m_pSurface );
	}
	m_pVertexBuffer->SetVertexBuffer( &m_stride, &m_nOffset );
	if ( m_pIndexBuffer )
	{
		m_pIndexBuffer->SetIndexBuffer( sizeof( WORD ), m_nIndexOffset );
		m_pMaterial->DrawIndexed( pDeviceContext, m_nIndices, m_nIndexOffset, m_nOffset );
	}
	else
	{
		m_pMaterial->Draw( pDeviceContext, m_nVertices, m_nOffset );
	}
}

CPlyMesh::CPlyMesh( ) : m_pSurface( nullptr )
{
}