#pragma once

#include <D3D11.h>
#include "IBuffer.h"

class D3D11BaseBuffer : public IBuffer
{
protected:
	ID3D11Buffer* CreateBuffer( ID3D11Device* pDevice, const D3D11_BUFFER_DESC* bufferDesc, const D3D11_SUBRESOURCE_DATA* initData );

public:
	virtual void SetIABuffer( ID3D11DeviceContext* pDeviceContext, const UINT* offset );
	virtual void SetVSBuffer( ID3D11DeviceContext* pDeviceContext, const UINT startSlot );
	virtual void SetPSBuffer( ID3D11DeviceContext* pDeviceContext, const UINT startSlot );

	D3D11BaseBuffer ( );
	virtual ~D3D11BaseBuffer ( );
};

