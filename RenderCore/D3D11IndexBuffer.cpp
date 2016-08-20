#include "stdafx.h"
#include "common.h"
#include "D3D11IndexBuffer.h"

#include "util_rendercore.h"

#include <D3D11.h>

void D3D11IndexBuffer::SetIABuffer( ID3D11DeviceContext* pDeviceContext, const UINT* offset ) const
{
	if ( pDeviceContext )
	{
		pDeviceContext->IASetIndexBuffer( m_pIndexBuffer.Get( ), sizeof( WORD ) == m_stride ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, *offset );
	}
}

bool D3D11IndexBuffer::CreateBuffer( ID3D11Device* pDevice, UINT stride, UINT numOfElement, const void* srcData )
{
	if ( stride != sizeof( WORD ) && stride != sizeof( DWORD ) || numOfElement <= 0 )
	{
		return false;
	}

	if ( pDevice )
	{
		m_stride = stride;
		m_numOfElement = numOfElement;
		m_bufferSize = m_stride * m_numOfElement;

		D3D11_BUFFER_DESC bufferDesc;
		::ZeroMemory( &bufferDesc, sizeof( D3D11_BUFFER_DESC ) );

		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bufferDesc.ByteWidth = stride * numOfElement;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;

		D3D11_SUBRESOURCE_DATA* pInitData = nullptr;
		D3D11_SUBRESOURCE_DATA initData;

		if ( srcData )
		{
			::ZeroMemory( &initData, sizeof( D3D11_SUBRESOURCE_DATA ) );

			initData.pSysMem = srcData;
			initData.SysMemPitch = 0;
			initData.SysMemSlicePitch = 0;

			pInitData = &initData;
		}

		m_pIndexBuffer = UTIL_BUFFER::CreateBuffer( pDevice, &bufferDesc, pInitData );

#ifdef _DEBUG
		SetDebugName( m_pIndexBuffer.Get( ), "Index Shader" );
#endif

		return m_pIndexBuffer ? true : false;
	}

	return false;
}

D3D11IndexBuffer::D3D11IndexBuffer( ) : m_pIndexBuffer( nullptr ),
m_stride( 0 ),
m_numOfElement( 0 )
{
}


D3D11IndexBuffer::~D3D11IndexBuffer ()
{
}
