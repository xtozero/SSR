#pragma once

#include "D3D11BaseBuffer.h"

class D3D11ConstantBuffer : public D3D11BaseBuffer
{
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_pConstantBuffer;
	UINT m_stride;
	UINT m_numOfElement;

public:
	virtual void SetVSBuffer( ID3D11DeviceContext* pDeviceContext, const UINT startSlot ) override;
	virtual void SetPSBuffer( ID3D11DeviceContext* pDeviceContext, const UINT startSlot ) override;
	virtual void*  LockBuffer( ID3D11DeviceContext* pDeviceContext ) override;
	virtual void UnLockBuffer( ID3D11DeviceContext* pDeviceContext ) override;

	bool CreateBuffer( ID3D11Device* pDevice, UINT stride, UINT numOfElement, const void* srcData );

	D3D11ConstantBuffer ();
	virtual ~D3D11ConstantBuffer ();
};

