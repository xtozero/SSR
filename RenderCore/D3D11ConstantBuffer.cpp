#include "stdafx.h"

#include "common.h"
#include "D3D11ConstantBuffer.h"

bool D3D11ConstantBuffer::CreateBuffer( ID3D11Device* m_pDevice, UINT stride, UINT numOfElement, const void* srcData )
{
	if ( stride <= 0 || numOfElement <= 0 )
	{
		return false;
	}

	if ( m_pDevice )
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

		D3D11_SUBRESOURCE_DATA initData;
		::ZeroMemory( &initData, sizeof( D3D11_SUBRESOURCE_DATA ) );

		initData.pSysMem = srcData;
		initData.SysMemPitch = 0;
		initData.SysMemSlicePitch = 0;

		m_pConstantBuffer = D3D11BaseBuffer::CreateBuffer( m_pDevice, &bufferDesc, &initData );

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
