#pragma once

#include "common.h"
#include "Buffer.h"
#include "GraphicsApiResource.h"

class IndexBuffer
{
public:
	aga::Buffer* Resource( );
	const aga::Buffer* Resource( ) const;

	IndexBuffer( uint32 numElement, const void* initData, bool isDWORD );

	IndexBuffer( ) = default;
	~IndexBuffer( ) = default;
	IndexBuffer( const IndexBuffer& ) = default;
	IndexBuffer& operator=( const IndexBuffer& ) = default;
	IndexBuffer( IndexBuffer&& ) = default;
	IndexBuffer& operator=( IndexBuffer&& ) = default;

	friend bool operator==( const IndexBuffer& lhs, const IndexBuffer& rhs )
	{
		return lhs.m_numElement == rhs.m_numElement &&
				lhs.m_isDWORD == rhs.m_isDWORD &&
				lhs.m_buffer == rhs.m_buffer;
	}

private:
	void InitResource( const void* initData );

	uint32 m_numElement = 0;
	bool m_isDWORD = false;
	RefHandle<aga::Buffer> m_buffer;
};