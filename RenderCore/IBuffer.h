#pragma once

struct ID3D11DeviceContext;

class IBuffer
{
protected:
	IBuffer () {}

public:
	virtual void SetIABuffer( ID3D11DeviceContext* pDeviceContext, const UINT* offset ) = 0;
	virtual void SetVSBuffer( ID3D11DeviceContext* pDeviceContext, const UINT startSlot ) = 0;
	virtual void SetPSBuffer( ID3D11DeviceContext* pDeviceContext, const UINT startSlot ) = 0;

	virtual void* LockBuffer( ID3D11DeviceContext* pDeviceContext ) = 0;
	virtual void UnLockBuffer( ID3D11DeviceContext* pDeviceContext ) = 0;

	virtual ~IBuffer () {}
};