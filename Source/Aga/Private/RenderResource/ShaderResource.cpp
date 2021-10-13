#include "stdafx.h"
#include "ShaderResource.h"

#include "InterfaceFactories.h"
#include "IRenderResourceManager.h"

namespace aga
{
	RefHandle<VertexShader> VertexShader::Create( const void* byteCode, size_t byteCodeSize )
	{
		return GetInterface<IResourceManager>( )->CreateVertexShader( byteCode, byteCodeSize );
	}

	RefHandle<PixelShader> PixelShader::Create( const void* byteCode, size_t byteCodeSize )
	{
		return GetInterface<IResourceManager>( )->CreatePixelShader( byteCode, byteCodeSize );
	}

	RefHandle<ComputeShader> ComputeShader::Create( const void* byteCode, size_t byteCodeSize )
	{
		return GetInterface<IResourceManager>( )->CreateComputeShader( byteCode, byteCodeSize );
	}
}
