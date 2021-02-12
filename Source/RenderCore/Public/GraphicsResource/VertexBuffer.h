#pragma once

#include "common.h"
#include "Buffer.h"
#include "GraphicsApiResource.h"

class VertexBuffer
{
public:
	static VertexBuffer Create( std::size_t elementSize, std::size_t numElement, const void* initData );

private:
	RefHandle<aga::Buffer> m_buffer;
};