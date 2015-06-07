#include "stdafx.h"
#include "common.h"
#include "D3D11IndexBuffer.h"

bool D3D11IndexBuffer::CreateBuffer( ID3D11Device* m_pDevice, UINT stride, UINT numOfElement, const void* srcData )
{
	if ( stride != sizeof( WORD ) || stride != sizeof( DWORD ) || numOfElement <= 0 )
	{
		return false;
	}

	if ( m_pDevice )
	{
		D3D11_BUFFER_DESC bufferDesc;
		::ZeroMemory( &bufferDesc, sizeof( D3D11_BUFFER_DESC ) );

		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bufferDesc.ByteWidth = stride * numOfElement;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;

		D3D11_SUBRESOURCE_DATA initData;
		::ZeroMemory( &initData, sizeof( D3D11_SUBRESOURCE_DATA ) );

		initData.pSysMem = srcData;
		initData.SysMemPitch = 0;
		initData.SysMemSlicePitch = 0;

		m_pIndexBuffer = D3D11BaseBuffer::CreateBuffer( m_pDevice, &bufferDesc, &initData );

		return m_pIndexBuffer ? true : false;
	}

	return false;
}

D3D11IndexBuffer::D3D11IndexBuffer( ) : m_pIndexBuffer( NULL ),
m_stride( 0 ),
m_numOfElement( 0 )
{
}


D3D11IndexBuffer::~D3D11IndexBuffer ()
{
	SAFE_RELEASE ( m_pIndexBuffer );
}
