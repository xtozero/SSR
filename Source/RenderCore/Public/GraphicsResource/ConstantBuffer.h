#pragma once

#include "common.h"
#include "Buffer.h"
#include "GraphicsApiResource.h"

class ConstantBuffer
{
public:
	void Update( const void* data, std::size_t size );
	void* Lock( );
	void Unlock( );
	void Bind( SHADER_TYPE shaderType, UINT slot );

	aga::Buffer* Resource( );
	const aga::Buffer* Resource( ) const;

	ConstantBuffer( std::size_t size );

	ConstantBuffer( ) = default;
	~ConstantBuffer( ) = default;
	ConstantBuffer( const ConstantBuffer& ) = default;
	ConstantBuffer& operator=( const ConstantBuffer& ) = default;
	ConstantBuffer( ConstantBuffer&& ) = default;
	ConstantBuffer& operator=( ConstantBuffer&& ) = default;

protected:
	void InitResource( std::size_t size );

private:
	RefHandle<aga::Buffer> m_buffer;
};