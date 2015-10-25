#include "stdafx.h"
#include "D3D11BaseBuffer.h"

void D3D11BaseBuffer::SetIABuffer( ID3D11DeviceContext* pDeviceContext, const UINT* offset )
{

}

void D3D11BaseBuffer::SetVSBuffer( ID3D11DeviceContext* pDeviceContext, const UINT startSlot )
{

}

void D3D11BaseBuffer::SetPSBuffer( ID3D11DeviceContext* pDeviceContext, const UINT startSlot )
{

}

ID3D11Buffer* D3D11BaseBuffer::CreateBuffer( ID3D11Device* pDevice, const D3D11_BUFFER_DESC* bufferDesc, const D3D11_SUBRESOURCE_DATA* initData )
{
	ID3D11Buffer* pBuffer;

	HRESULT hr = pDevice->CreateBuffer( bufferDesc, initData, &pBuffer );

	return SUCCEEDED ( hr ) ? pBuffer : NULL;
}

D3D11BaseBuffer::D3D11BaseBuffer ( )
{
}

D3D11BaseBuffer::~D3D11BaseBuffer ( )
{
}
