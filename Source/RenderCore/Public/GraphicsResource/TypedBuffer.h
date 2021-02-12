#pragma once

#include "ConstantBuffer.h"
#include "GraphicsApiResource.h"
#include "VertexBuffer.h"

template <typename T>
class TypedConstatBuffer
{
public:
	static TypedConstatBuffer Create( )
	{
		return TypedConstatBuffer<T>( ConstantBuffer::Create( sizeof( T ) ) );
	}

	void Update( const T& data )
	{
		m_cb.Update( &data, sizeof( T ) );
	}

	void Bind( SHADER_TYPE shaderType, UINT slot )
	{
		m_cb.Bind( shaderType, slot );
	}

	TypedConstatBuffer( ) = default;

private:
	TypedConstatBuffer( ConstantBuffer&& cb )
	{
		m_cb = std::move( cb );
	}

	ConstantBuffer m_cb;
};

template <typename T>
class TypeVertexBuffer
{
public:
	void Initialize( std::size_t numElement, const void* initData )
	{
		m_vb = VertexBuffer::Create( sizeof( T ), numElement, initData );
	}

private:
	VertexBuffer m_vb;
};