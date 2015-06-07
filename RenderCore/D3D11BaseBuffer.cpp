#include "stdafx.h"
#include "D3D11BaseBuffer.h"

ID3D11Buffer* D3D11BaseBuffer::CreateBuffer ( ID3D11Device* m_pDevice, const D3D11_BUFFER_DESC* bufferDesc, const D3D11_SUBRESOURCE_DATA* initData )
{
	ID3D11Buffer* pBuffer;

	HRESULT hr = m_pDevice->CreateBuffer ( bufferDesc, initData, &pBuffer );

	return SUCCEEDED ( hr ) ? pBuffer : NULL;
}

D3D11BaseBuffer::D3D11BaseBuffer ( )
{
}

D3D11BaseBuffer::~D3D11BaseBuffer ( )
{
}
