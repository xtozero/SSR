#pragma once

#include "Resource.h"

class IBuffer
{
protected:
	IBuffer( ) : m_bufferSize( 0 ) {}

public:
	virtual void SetVertexBuffer( const UINT* pStride, const UINT* pOffset ) const = 0;
	virtual void SetIndexBuffer( UINT stride, UINT offset ) const = 0;
	virtual void SetVSBuffer( const UINT startSlot ) const = 0;
	virtual void SetPSBuffer( const UINT startSlot ) const = 0;

	virtual void* LockBuffer( UINT subResource = 0 ) = 0;
	virtual void UnLockBuffer( UINT subResource = 0 ) = 0;
	UINT Size( ) const { return m_bufferSize; }

	virtual ~IBuffer( ) = default;

protected:
	UINT m_bufferSize;
};