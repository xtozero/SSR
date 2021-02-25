#pragma once

#include "common.h"
#include "Buffer.h"
#include "GraphicsApiResource.h"

class ConstantBuffer
{
public:
	static ConstantBuffer Create( std::size_t size );
	void Update( const void* data, std::size_t size );
	void Bind( SHADER_TYPE shaderType, UINT slot );

	operator aga::Buffer*( )
	{
		return m_buffer.Get( );
	}

private:
	RefHandle<aga::Buffer> m_buffer;
};