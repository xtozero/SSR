#pragma once

#include <wrl/client.h>
#include "IBuffer.h"

struct ID3D11Buffer;
struct ID3D11Device;
struct D3D11_BUFFER_DESC;
struct D3D11_SUBRESOURCE_DATA;
struct ID3D11DeviceContext;

namespace UTIL_BUFFER
{
	Microsoft::WRL::ComPtr<ID3D11Buffer> CreateBuffer( ID3D11Device* pDevice, const D3D11_BUFFER_DESC* bufferDesc, const D3D11_SUBRESOURCE_DATA* initData );
}

class D3D11BaseBuffer : public IBuffer
{
public:
	virtual void SetIABuffer( ID3D11DeviceContext* pDeviceContext, const UINT* offset ) const override;
	virtual void SetVSBuffer( ID3D11DeviceContext* pDeviceContext, const UINT startSlot ) const override;
	virtual void SetPSBuffer( ID3D11DeviceContext* pDeviceContext, const UINT startSlot ) const override;

	virtual void* LockBuffer( ID3D11DeviceContext* pDeviceContext ) override;
	virtual void UnLockBuffer( ID3D11DeviceContext* pDeviceContext ) override;

	D3D11BaseBuffer ( );
	virtual ~D3D11BaseBuffer( );
};

