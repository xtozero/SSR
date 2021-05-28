#pragma once

#include "common.h"
#include "Buffer.h"
#include "GraphicsApiResource.h"

class IndexBuffer
{
public:
	aga::Buffer* Resource( );
	const aga::Buffer* Resource( ) const;

	IndexBuffer( std::size_t numElement, const void* initData, bool isDWORD );

	IndexBuffer( ) = default;
	~IndexBuffer( ) = default;
	IndexBuffer( const IndexBuffer& ) = default;
	IndexBuffer& operator=( const IndexBuffer& ) = default;
	IndexBuffer( IndexBuffer&& ) = default;
	IndexBuffer& operator=( IndexBuffer&& ) = default;

private:
	void InitResource( const void* initData );

	std::size_t m_numElement = 0;
	bool m_isDWORD = false;
	RefHandle<aga::Buffer> m_buffer;
};