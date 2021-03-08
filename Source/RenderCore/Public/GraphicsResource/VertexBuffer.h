#pragma once

#include "common.h"
#include "Buffer.h"
#include "GraphicsApiResource.h"

class VertexBuffer
{
public:
	static VertexBuffer Create( std::size_t elementSize, std::size_t numElement, const void* initData );

	operator aga::Buffer*( )
	{
		return m_buffer.Get( );
	}

private:
	RefHandle<aga::Buffer> m_buffer;
};