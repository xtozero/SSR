#pragma once

#include "../CommonRenderer/IBuffer.h"

#include <wrl/client.h>

struct ID3D11Buffer;
struct ID3D11Device;
struct ID3D11DeviceContext;

class CD3D11Buffer : public IBuffer
{
public:
	bool Create( ID3D11Device& device, const BUFFER_TRAIT& trait );

	virtual void SetVertexBuffer( const UINT* pStride, const UINT* pOffset ) const override;
	virtual void SetIndexBuffer( UINT stride, UINT offset ) const override;
	virtual void SetVSBuffer( const UINT startSlot ) const override;
	virtual void SetPSBuffer( const UINT startSlot ) const override;
	virtual void SetCSBuffer( const UINT startSlot ) const override;

	virtual void* Get( ) const override { return m_buffer.Get( ); };

	virtual void* LockBuffer( UINT lockFlag = BUFFER_LOCKFLAG::WRITE_DISCARD, UINT subResource = 0 ) override;
	virtual void UnLockBuffer( UINT subResource = 0 ) override;

	explicit CD3D11Buffer( ID3D11DeviceContext& deviceContext );

private:
	ID3D11DeviceContext& m_deviceContext;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
};
