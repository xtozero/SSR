#pragma once

#include "common.h"
#include "GraphicsApiResource.h"

class IndexBuffer : public DeviceDependantResource
{
public:
	AGA_DLL static RefHandle<IndexBuffer> Create( std::size_t numElement, const void* initData, bool isDWORD );
};