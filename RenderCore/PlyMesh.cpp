#include "stdafx.h"
#include "common.h"
#include "Direct3D11.h"

#include "MaterialSystem.h"
#include "Material.h"

#include "PlyMesh.h"

extern IRenderer* g_pRenderer;

bool CPlyMesh::Load( )
{
	m_primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	UINT stride = sizeof( D3DXVECTOR3 );

	m_pVertexBuffer = g_pRenderer->CreateVertexBuffer( stride, m_nVertices, m_pModelData );

	if ( !m_pVertexBuffer )
	{
		return false;
	}

	//temp... will be change
	ON_FAIL_RETURN( LoadMaterial( _T( "tutorial" ) ) );
}

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

	m_pVertexBuffer->SetIABuffer( pDeviceContext, &m_nOffset );
	m_pMaterial->SetShader( pDeviceContext );
	m_pMaterial->SetPrimitiveTopology( pDeviceContext, m_primitiveTopology );
	m_pMaterial->Draw( pDeviceContext, m_nVertices, m_nOffset );
}

CPlyMesh::CPlyMesh( )
{
}


CPlyMesh::~CPlyMesh( )
{
}
