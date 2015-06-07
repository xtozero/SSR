#pragma once

#include "D3D11BaseBuffer.h"

class D3D11ConstantBuffer : public D3D11BaseBuffer
{
private:
	ID3D11Buffer* m_pConstantBuffer;
	UINT m_stride;
	UINT m_numOfElement;

public:
	virtual bool CreateBuffer( ID3D11Device* m_pDevice, UINT stride, UINT numOfElement, const void* srcData );

	D3D11ConstantBuffer ();
	virtual ~D3D11ConstantBuffer ();
};

