#include "stdafx.h"

#include "common.h"
#include "D3D11ConstantBuffer.h"

void D3D11ConstantBuffer::SetVSBuffer( ID3D11DeviceContext* pDeviceContext, const UINT startSlot )
{
	if ( pDeviceContext )
	{
		pDeviceContext->VSSetConstantBuffers( startSlot, 1, &m_pConstantBuffer );
	}
}

void D3D11ConstantBuffer::SetPSBuffer( ID3D11DeviceContext* pDeviceContext, const UINT startSlot )
{
	if ( pDeviceContext )
	{
		pDeviceContext->PSSetConstantBuffers( startSlot, 1, &m_pConstantBuffer );
	}
}

void* D3D11ConstantBuffer::LockBuffer( ID3D11DeviceContext* pDeviceContext )
{
	D3D11_MAPPED_SUBRESOURCE subResource;
	::ZeroMemory( &subResource, sizeof( D3D11_MAPPED_SUBRESOURCE ) );

	if ( SUCCEEDED( pDeviceContext->Map( m_pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource ) ) )
	{
		return subResource.pData;
	}

	return nullptr;
}

void D3D11ConstantBuffer::UnLockBuffer( ID3D11DeviceContext* pDeviceContext )
{
	pDeviceContext->Unmap( m_pConstantBuffer, 0 );
}

bool D3D11ConstantBuffer::CreateBuffer( ID3D11Device* pDevice, UINT stride, UINT numOfElement, const void* srcData )
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

		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.ByteWidth = stride * numOfElement;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;

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

		m_pConstantBuffer = UTIL_BUFFER::CreateBuffer( pDevice, &bufferDesc, pInitData );

		return m_pConstantBuffer ? true : false;
	}

	return false;
}

D3D11ConstantBuffer::D3D11ConstantBuffer ()
{
}


D3D11ConstantBuffer::~D3D11ConstantBuffer ()
{
	SAFE_RELEASE ( m_pConstantBuffer );
}
