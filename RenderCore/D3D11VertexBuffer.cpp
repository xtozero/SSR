#include "stdafx.h"

#include "common.h"
#include <d3dcommon.h>
#include "D3D11VertexBuffer.h"
#include "util_rendercore.h"

void D3D11VertexBuffer::SetIABuffer( ID3D11DeviceContext* pDeviceContext, const UINT* offset )
{
	if ( pDeviceContext )
	{
		pDeviceContext->IASetVertexBuffers( 0, 1, m_pVertexBuffer.GetAddressOf( ), &m_stride, offset );
	}
}

bool D3D11VertexBuffer::CreateBuffer( ID3D11Device* pDevice, UINT stride, UINT numOfElement, const void* srcData )
{
	if ( stride <= 0 || numOfElement <= 0 )
	{
		return false;
	}

	if ( pDevice )
	{
		m_stride = stride;
		m_numOfElement = numOfElement;

		D3D11_BUFFER_DESC bufferDesc;
		::ZeroMemory( &bufferDesc, sizeof( D3D11_BUFFER_DESC ) );

		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.ByteWidth = stride * numOfElement;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;

		D3D11_SUBRESOURCE_DATA* pInitData = nullptr;

		if ( srcData )
		{
			D3D11_SUBRESOURCE_DATA initData;
			::ZeroMemory( &initData, sizeof( D3D11_SUBRESOURCE_DATA ) );

			initData.pSysMem = srcData;
			initData.SysMemPitch = 0;
			initData.SysMemSlicePitch = 0;

			pInitData = &initData;
		}

		m_pVertexBuffer = UTIL_BUFFER::CreateBuffer( pDevice, &bufferDesc, pInitData );

#ifdef _DEBUG
		SetDebugName( m_pVertexBuffer.Get( ), "Vertex Buffer" );
#endif

		return m_pVertexBuffer ? true : false;
	}

	return false;
}

D3D11VertexBuffer::D3D11VertexBuffer( ) : m_pVertexBuffer( nullptr ),
m_stride( 0 ),
m_numOfElement( 0 )
{
}

D3D11VertexBuffer::~D3D11VertexBuffer( )
{
}
