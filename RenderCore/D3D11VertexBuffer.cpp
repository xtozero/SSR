#include "stdafx.h"

#include "common.h"
#include "D3D11VertexBuffer.h"

bool D3D11VertexBuffer::CreateBuffer ( const D3D11_CREATE_VB_TRAIT& trait )
{
	m_primTopology = trait.m_primTopology;

	ON_FAIL_RETURN ( SUCCEEDED ( trait.m_pDevice->CreateInputLayout ( trait.m_pInputElementDesc,
		trait.m_numInputElement,
		trait.m_pD3DBlob->GetBufferPointer (),
		trait.m_pD3DBlob->GetBufferSize (),
		&m_inputLayout ) ) );

	ON_SUCCESS_RETURE ( SUCCEEDED ( trait.m_pDevice->CreateBuffer ( &trait.m_vbDesc,
		NULL,
		&m_pVertexBuffer ) ) );
}

void D3D11VertexBuffer::SetBuffer ( const D3D11_SET_VB_TRAIT& trait )
{
	trait.m_pDeviceContext->IASetVertexBuffers ( trait.m_slot,
		1,
		&m_pVertexBuffer,
		&m_strides,
		&m_offset );
}

D3D11VertexBuffer::D3D11VertexBuffer () : m_pVertexBuffer ( NULL ),
m_primTopology ( D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED ),
m_inputLayout ( NULL ),
m_strides ( 0 ),
m_offset ( 0 )
{
}


D3D11VertexBuffer::~D3D11VertexBuffer ()
{
	SAFE_RELEASE( m_pVertexBuffer );
}
