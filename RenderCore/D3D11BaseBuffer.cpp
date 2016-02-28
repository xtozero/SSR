#include "stdafx.h"
#include "D3D11BaseBuffer.h"

#include <D3D11.h>

namespace UTIL_BUFFER
{
	Microsoft::WRL::ComPtr<ID3D11Buffer> CreateBuffer( ID3D11Device* pDevice, const D3D11_BUFFER_DESC* bufferDesc, const D3D11_SUBRESOURCE_DATA* initData )
	{
		Microsoft::WRL::ComPtr<ID3D11Buffer> pBuffer( nullptr );

		HRESULT hr = pDevice->CreateBuffer( bufferDesc, initData, &pBuffer );

		return SUCCEEDED( hr ) ? pBuffer : nullptr;
	}
}

void D3D11BaseBuffer::SetIABuffer( ID3D11DeviceContext*, const UINT* )
{

}

void D3D11BaseBuffer::SetVSBuffer( ID3D11DeviceContext*, const UINT )
{

}

void D3D11BaseBuffer::SetPSBuffer( ID3D11DeviceContext*, const UINT )
{

}

void* D3D11BaseBuffer::LockBuffer( ID3D11DeviceContext* )
{
	return nullptr;
}

void D3D11BaseBuffer::UnLockBuffer( ID3D11DeviceContext* )
{

}

D3D11BaseBuffer::D3D11BaseBuffer ( )
{
}

D3D11BaseBuffer::~D3D11BaseBuffer ( )
{
}
