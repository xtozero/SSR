#pragma once

#include "D3D11BaseBuffer.h"

class D3D11IndexBuffer : public D3D11BaseBuffer
{
private:
	ID3D11Buffer* m_pIndexBuffer;
	UINT m_stride;
	UINT m_numOfElement;

public:
	bool CreateBuffer( ID3D11Device* m_pDevice, UINT stride, UINT numOfElement, const void* srcData );

	D3D11IndexBuffer ();
	~D3D11IndexBuffer ();
};

