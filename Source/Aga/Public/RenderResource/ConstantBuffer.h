#pragma once

#include "common.h"
#include "GraphicsApiResource.h"

class ConstantBuffer : public DeviceDependantResource
{
public:
	AGA_DLL static RefHandle<ConstantBuffer> Create( std::size_t size );
};