#pragma once

#include "common.h"
#include "GraphicsApiResource.h"

class VertexBuffer : public DeviceDependantResource
{
public:
	AGA_DLL static RefHandle<VertexBuffer> Create( std::size_t elementSize, std::size_t numElement, const void* initData );
};