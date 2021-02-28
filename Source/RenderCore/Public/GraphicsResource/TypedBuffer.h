#pragma once

#include "ConstantBuffer.h"
#include "GraphicsApiResource.h"
#include "UploadBuffer.h"
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
	TypedConstatBuffer( const TypedConstatBuffer& ) = default;
	TypedConstatBuffer& operator=( const TypedConstatBuffer& ) = default;
	TypedConstatBuffer( TypedConstatBuffer&& ) = default;
	TypedConstatBuffer& operator=( TypedConstatBuffer&& ) = default;

private:
	TypedConstatBuffer( ConstantBuffer&& cb )
	{
		m_cb = std::move( cb );
	}

	ConstantBuffer m_cb;
};

template <typename T>
class TypedVertexBuffer
{
public:
	static TypedVertexBuffer Create( std::size_t numElement, const void* initData )
	{
		assert( numElement > 0 );
		return TypedVertexBuffer<T>( VertexBuffer::Create( sizeof( T ), numElement, initData ) );
	}

	TypedVertexBuffer( ) = default;
	TypedVertexBuffer( const TypedVertexBuffer& ) = default;
	TypedVertexBuffer& operator=( const TypedVertexBuffer& ) = default;
	TypedVertexBuffer( TypedVertexBuffer&& ) = default;
	TypedVertexBuffer& operator=( TypedVertexBuffer&& ) = default;

	operator const VertexBuffer&( ) const
	{
		return m_vb;
	}

private:
	TypedVertexBuffer( VertexBuffer&& vb )
	{
		m_vb = std::move( vb );
	}

	VertexBuffer m_vb;
};

template <typename T>
class TypedBuffer
{
public:
	static TypedBuffer Create( std::size_t numElement, const void* initData = nullptr )
	{
		TypedBuffer tb;

		if ( numElement > 0 )
		{
			BUFFER_TRAIT trait = {
			static_cast<UINT>( sizeof( T ) ),
			static_cast<UINT>( numElement ),
			RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::GPU_WRITE,
			RESOURCE_BIND_TYPE::SHADER_RESOURCE | RESOURCE_BIND_TYPE::RANDOM_ACCESS,
			RESOURCE_MISC::BUFFER_STRUCTURED };

			tb.m_buffer = GetInterface<IAga>( )->CreateBuffer( trait, initData );
			if ( IsInRenderThread( ) )
			{
				tb.m_buffer->Init( );
			}
			else
			{
				EnqueueRenderTask( [buffer = tb.m_buffer]( )
				{
					buffer->Init( );
				} );
			}
		}
		
		tb.m_numElement = numElement;
		return tb;
	}

	void Resize( std::size_t newNumElement )
	{
		if ( m_numElement == 0 )
		{
			( *this ) = TypedBuffer::Create( newNumElement );
		}
		else if ( newNumElement > m_numElement )
		{
			TypedBuffer newBuffer = TypedBuffer::Create( newNumElement );
			GetInterface<IAga>( )->Copy( newBuffer.m_buffer, m_buffer, Size( ) );

			( *this ) = std::move( newBuffer );
		}
	}

	std::size_t Size( ) const { return sizeof( T ) * m_numElement; }

	TypedBuffer( ) = default;
	TypedBuffer( const TypedBuffer& ) = default;
	TypedBuffer& operator=( const TypedBuffer& ) = default;
	TypedBuffer( TypedBuffer&& ) = default;
	TypedBuffer& operator=( TypedBuffer&& ) = default;

	operator aga::Buffer*( )
	{
		return m_buffer.Get( );
	}

private:
	TypedBuffer( VertexBuffer&& vb )
	{
		m_vb = std::move( vb );
	}

	RefHandle<aga::Buffer> m_buffer;
	std::size_t m_numElement = 0;
};

template <typename T>
class TypedUploadBuffer : public UploadBuffer
{
public:
	static TypedUploadBuffer Create( std::size_t numElement, const void* initData )
	{
		assert( numElement > 0 );
		return TypedUploadBuffer<T>( UploadBuffer::Create( sizeof( T ), numElement, initData ) );
	}

	TypedUploadBuffer( )
	{
		m_elementSize = sizeof( T );
	}

	TypedUploadBuffer( const TypedUploadBuffer& ) = default;
	TypedUploadBuffer& operator=( const TypedUploadBuffer& ) = default;
	TypedUploadBuffer( TypedUploadBuffer&& ) = default;
	TypedUploadBuffer& operator=( TypedUploadBuffer&& ) = default;
};
