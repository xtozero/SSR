#pragma once

#include "common.h"
#include "Buffer.h"
#include "GraphicsApiResource.h"

class ComputeShader;
class PixelShader;
class VertexShader;

class ConstantBuffer
{
public:
	void Update( const void* data, std::size_t size );
	void* Lock( );
	void Unlock( );

	template <typename ShaderType>
	void Bind( ShaderType& shader, UINT slot )
	{
		BindImple( shader, slot );
	}

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
	void BindImple( VertexShader& shader, UINT slot );
	void BindImple( PixelShader& shader, UINT slot );
	void BindImple( ComputeShader& shader, UINT slot );

	RefHandle<aga::Buffer> m_buffer;
};