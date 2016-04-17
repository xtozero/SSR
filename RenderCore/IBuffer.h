#pragma once

struct ID3D11DeviceContext;

class IBuffer
{
protected:
	IBuffer( ) : m_bufferSize( 0 ) {}

public:
	virtual void SetIABuffer( ID3D11DeviceContext* pDeviceContext, const UINT* offset ) const = 0;
	virtual void SetVSBuffer( ID3D11DeviceContext* pDeviceContext, const UINT startSlot ) const = 0;
	virtual void SetPSBuffer( ID3D11DeviceContext* pDeviceContext, const UINT startSlot ) const = 0;

	virtual void* LockBuffer( ID3D11DeviceContext* pDeviceContext ) = 0;
	virtual void UnLockBuffer( ID3D11DeviceContext* pDeviceContext ) = 0;
	UINT Size( ) const { return m_bufferSize; }

	virtual ~IBuffer( ) = default;

protected:
	UINT m_bufferSize;
};