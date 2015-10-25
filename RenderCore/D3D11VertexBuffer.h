#pragma once

#include "D3D11BaseBuffer.h"

class D3D11VertexBuffer : public D3D11BaseBuffer
{
private:
	ID3D11Buffer* m_pVertexBuffer;
	UINT m_stride;
	UINT m_numOfElement;

public:
	virtual void SetIABuffer( ID3D11DeviceContext* pDeviceContext, const UINT* offset );

	bool CreateBuffer( ID3D11Device* pDevice, UINT stride, UINT numOfElement, const void* srcData );

	D3D11VertexBuffer ();
	~D3D11VertexBuffer ();
};

