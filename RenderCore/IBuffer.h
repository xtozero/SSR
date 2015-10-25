#pragma once

#include <d3d11.h>

class IBuffer
{
protected:
	IBuffer () {}

public:
	virtual void SetIABuffer( ID3D11DeviceContext* pDeviceContext, const UINT* offset ) = 0;
	virtual void SetVSBuffer( ID3D11DeviceContext* pDeviceContext, const UINT startSlot ) = 0;
	virtual void SetPSBuffer( ID3D11DeviceContext* pDeviceContext, const UINT startSlot ) = 0;

	virtual ~IBuffer () {}
};