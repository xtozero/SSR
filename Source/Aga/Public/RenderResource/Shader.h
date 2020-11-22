#pragma once

#include "common.h"
#include "GraphicsApiResource.h"

class VertexShader : public DeviceDependantResource
{
public:
	AGA_DLL static VertexShader* Create( const void* byteCode, std::size_t byteCodeSize );
};

class PixelShader : public DeviceDependantResource
{
public:
	AGA_DLL static PixelShader* Create( const void* byteCode, std::size_t byteCodeSize );
};