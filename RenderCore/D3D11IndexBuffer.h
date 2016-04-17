#pragma once

#include "D3D11BaseBuffer.h"

class D3D11IndexBuffer : public D3D11BaseBuffer
{
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_pIndexBuffer;
	UINT m_stride;
	UINT m_numOfElement;

public:
	virtual void SetIABuffer( ID3D11DeviceContext* pDeviceContext, const UINT* offset ) const override;

	bool CreateBuffer( ID3D11Device* pDevice, UINT stride, UINT numOfElement, const void* srcData );

	D3D11IndexBuffer ();
	~D3D11IndexBuffer ();
};

