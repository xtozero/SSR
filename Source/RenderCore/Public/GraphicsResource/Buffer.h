#pragma once

#include "ConstantBuffer.h"
#include "GraphicsApiResource.h"
#include "VertexBuffer.h"

template <typename T>
class TypeConstatBuffer
{
public:
	static TypeConstatBuffer Create( )
	{
		return TypeConstatBuffer<T>( ConstantBuffer::Create( sizeof( T ) ) );
	}

	TypeConstatBuffer( ) = default;

private:
	TypeConstatBuffer( RefHandle<ConstantBuffer>&& cb )
	{
		m_cb = std::move( cb );
	}

	RefHandle<ConstantBuffer> m_cb;
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
	RefHandle<VertexBuffer> m_vb;
};