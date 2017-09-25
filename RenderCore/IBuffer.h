#pragma once

#include <tuple>

namespace BUFFER_TYPE
{
	enum
	{
		// from D3D11
		VERTEX_BUFFER = 0x01,
		INDEX_BUFFER = 0x02,
		CONSTANT_BUFFER = 0x04,
		SHADER_RESOURCE = 0x08,
		STREAM_OUTPUT = 0x10,
		RENDER_TARGET = 0x20,
		DEPTH_STENCIL = 0x40,
		UNORDERED_ACCESS = 0x80
	};
}

namespace BUFFER_MISC
{
	enum 	
	{
		BUFFER_STRUCTURED = 0x01
	};
}

namespace BUFFER_ACCESS_FLAG
{
	enum
	{
		ACCESS_NONE = 0x00,
		GPU_READ = 0x01,
		GPU_WRITE = 0x02,
		CPU_READ = 0x04,
		CPU_WRITE = 0x08,
	};
}

struct BUFFER_TRAIT
{
	UINT m_stride;
	UINT m_count;
	UINT m_access;
	UINT m_bufferType;
	UINT m_miscFlag;
	void* m_srcData;
	UINT m_pitch;
	UINT m_slicePitch;
};

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