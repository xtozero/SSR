#include "stdafx.h"
#include "common.h"
#include "CommonMeshDefine.h"
#include "Direct3D11.h"

#include "MaterialSystem.h"
#include "Material.h"

#include "PlyMesh.h"

extern IRenderer* g_pRenderer;

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
	m_pVertexBuffer->SetIABuffer( pDeviceContext, &m_nOffset );
	if ( m_pIndexBuffer )
	{
		m_pIndexBuffer->SetIABuffer( pDeviceContext, &m_nIndexOffset );
		m_pMaterial->DrawIndexed( pDeviceContext, m_nIndices, m_nIndexOffset, m_nOffset );
	}
	else
	{
		m_pMaterial->Draw( pDeviceContext, m_nVertices, m_nOffset );
	}
}

CPlyMesh::CPlyMesh( )
{
}


CPlyMesh::~CPlyMesh( )
{
}
