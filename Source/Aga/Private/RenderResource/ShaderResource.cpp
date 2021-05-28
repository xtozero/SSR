#include "stdafx.h"
#include "ShaderResource.h"

#include "Core/InterfaceFactories.h"
#include "IAga.h"

namespace aga
{
	RefHandle<VertexShader> VertexShader::Create( const void* byteCode, std::size_t byteCodeSize )
	{
		return GetInterface<IAga>( )->CreateVertexShader( byteCode, byteCodeSize );
	}

	RefHandle<PixelShader> PixelShader::Create( const void* byteCode, std::size_t byteCodeSize )
	{
		return GetInterface<IAga>( )->CreatePixelShader( byteCode, byteCodeSize );
	}

	RefHandle<ComputeShader> ComputeShader::Create( const void* byteCode, std::size_t byteCodeSize )
	{
		return GetInterface<IAga>( )->CreateComputeShader( byteCode, byteCodeSize );
	}
}
