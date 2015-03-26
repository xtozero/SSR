#include "stdafx.h"
#include "D3D11IndexBuffer.h"

bool D3D11IndexBuffer::CreateBuffer ( const D3D11_CREATE_IB_TRAIT& trait )
{
	return SUCCEEDED ( trait.m_pDevice->CreateBuffer ( &trait.m_ibDesc,
		NULL,
		&m_pIndexBuffer ) );
}

D3D11IndexBuffer::D3D11IndexBuffer ()
{
}


D3D11IndexBuffer::~D3D11IndexBuffer ()
{
	m_pIndexBuffer->Release ();
}
