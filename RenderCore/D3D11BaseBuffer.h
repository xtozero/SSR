#pragma once

#include <D3D11.h>
#include "IBuffer.h"

namespace UTIL_BUFFER
{
	ID3D11Buffer* CreateBuffer( ID3D11Device* pDevice, const D3D11_BUFFER_DESC* bufferDesc, const D3D11_SUBRESOURCE_DATA* initData );
}

class D3D11BaseBuffer : public IBuffer
{
public:
	virtual void SetIABuffer( ID3D11DeviceContext* pDeviceContext, const UINT* offset ) override;
	virtual void SetVSBuffer( ID3D11DeviceContext* pDeviceContext, const UINT startSlot ) override;
	virtual void SetPSBuffer( ID3D11DeviceContext* pDeviceContext, const UINT startSlot ) override;

	virtual void* LockBuffer( ID3D11DeviceContext* pDeviceContext ) override;
	virtual void UnLockBuffer( ID3D11DeviceContext* pDeviceContext ) override;

	D3D11BaseBuffer ( );
	virtual ~D3D11BaseBuffer ( );
};

