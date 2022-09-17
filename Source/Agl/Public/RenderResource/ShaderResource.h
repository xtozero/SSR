#pragma once

#include "common.h"
#include "GraphicsApiResource.h"
#include "SizedTypes.h"

namespace agl
{
	class VertexShader : public DeviceDependantResource
	{
	public:
		AGL_DLL static RefHandle<VertexShader> Create( const void* byteCode, size_t byteCodeSize );
	};

	class GeometryShader : public DeviceDependantResource
	{
	public:
		AGL_DLL static RefHandle<GeometryShader> Create( const void* byteCode, size_t byteCodeSize );
	};

	class PixelShader : public DeviceDependantResource
	{
	public:
		AGL_DLL static RefHandle<PixelShader> Create( const void* byteCode, size_t byteCodeSize );
	};

	class ComputeShader : public DeviceDependantResource
	{
	public:
		AGL_DLL static RefHandle<ComputeShader> Create( const void* byteCode, size_t byteCodeSize );
	};
}