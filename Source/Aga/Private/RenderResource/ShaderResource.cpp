#include "stdafx.h"
#include "ShaderResource.h"

#include "Core/InterfaceFactories.h"
#include "IAga.h"

namespace aga
{
	AGA_DLL VertexShader* VertexShader::Create( const void* byteCode, std::size_t byteCodeSize )
	{
		return GetInterface<IAga>( )->CreateVertexShader( byteCode, byteCodeSize );
	}

	AGA_DLL PixelShader* PixelShader::Create( const void* byteCode, std::size_t byteCodeSize )
	{
		return GetInterface<IAga>( )->CreatePixelShader( byteCode, byteCodeSize );
	}
}
