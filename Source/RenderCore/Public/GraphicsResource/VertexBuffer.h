#pragma once

#include "common.h"
#include "Buffer.h"
#include "GraphicsApiResource.h"

class VertexBuffer
{
public:
	void* Lock( );
	void Unlock( );

	std::size_t Size( ) const
	{
		return m_size;
	}

	aga::Buffer* Resource( );
	const aga::Buffer* Resource( ) const;

	VertexBuffer( std::size_t elementSize, std::size_t numElement, const void* initData, bool isDynamic = false );

	VertexBuffer( ) = default;
	~VertexBuffer( ) = default;
	VertexBuffer( const VertexBuffer& ) = default;
	VertexBuffer& operator=( const VertexBuffer& ) = default;
	VertexBuffer( VertexBuffer&& ) = default;
	VertexBuffer& operator=( VertexBuffer&& ) = default;

protected:
	void InitResource( std::size_t elementSize, std::size_t numElement, const void* initData );

	RefHandle<aga::Buffer> m_buffer;
	std::size_t m_size = 0;
	bool m_isDynamic = false;
};