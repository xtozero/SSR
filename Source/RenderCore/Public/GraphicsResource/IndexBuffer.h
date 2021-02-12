#pragma once

#include "common.h"
#include "Buffer.h"
#include "GraphicsApiResource.h"

class IndexBuffer
{
public:
	static IndexBuffer Create( std::size_t numElement, const void* initData, bool isDWORD );

private:
	RefHandle<aga::Buffer> m_buffer;
};