#include "stdafx.h"
#include "D3D11VertexBuffer.h"

bool D3D11VertexBuffer::CreateBuffer ( const D3D11_CREATE_VB_TRAIT& trait )
{
	return SUCCEEDED ( trait.m_pDevice->CreateBuffer ( &trait.m_vbDesc, 
														NULL,
														&m_pVertexBuffer ) );
}

D3D11VertexBuffer::D3D11VertexBuffer ()
{
}


D3D11VertexBuffer::~D3D11VertexBuffer ()
{
	m_pVertexBuffer->Release ();
}
