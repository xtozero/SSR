#pragma once

#include "AbstractGraphicsInterface.h"
#include "ConstantBuffer.h"
#include "GraphicsApiResource.h"
#include "UploadBuffer.h"
#include "VertexBuffer.h"

template <typename T>
class TypedConstatBuffer : public ConstantBuffer
{
public:
	void Update( const T& data )
	{
		ConstantBuffer::Update( &data, sizeof( T ) );
	}

	TypedConstatBuffer( ) : ConstantBuffer( sizeof( T ) ) {}
	~TypedConstatBuffer( ) = default;
	TypedConstatBuffer( const TypedConstatBuffer& ) = default;
	TypedConstatBuffer& operator=( const TypedConstatBuffer& ) = default;
	TypedConstatBuffer( TypedConstatBuffer&& ) = default;
	TypedConstatBuffer& operator=( TypedConstatBuffer&& ) = default;
};

template <typename T>
class TypedVertexBuffer : public VertexBuffer
{
public:
	TypedVertexBuffer( std::size_t numElement, const void* initData ) : VertexBuffer( sizeof( T ), numElement, initData, false ) { }

	TypedVertexBuffer( ) = default;
	~TypedVertexBuffer( ) = default;
	TypedVertexBuffer( const TypedVertexBuffer& ) = default;
	TypedVertexBuffer& operator=( const TypedVertexBuffer& ) = default;
	TypedVertexBuffer( TypedVertexBuffer&& ) = default;
	TypedVertexBuffer& operator=( TypedVertexBuffer&& ) = default;
};

template <typename T>
class TypedBuffer
{
public:
	void Resize( std::size_t newNumElement, bool copyPreviousData )
	{
		if ( newNumElement > m_numElement )
		{
			TypedBuffer newBuffer = TypedBuffer( newNumElement );

			if ( copyPreviousData )
			{
				GraphicsInterface( ).Copy( newBuffer.m_buffer, m_buffer, Size( ) );
			}

			( *this ) = std::move( newBuffer );
		}
	}

	std::size_t Size( ) const { return sizeof( T ) * m_numElement; }

	aga::Buffer* Resource( )
	{
		return m_buffer.Get( );
	}

	const aga::Buffer* Resource( ) const
	{
		return m_buffer.Get( );
	}

	aga::ShaderResourceView* SRV( )
	{
		return m_buffer.Get( ) ? m_buffer->SRV( ) : nullptr;
	}

	const aga::ShaderResourceView* SRV( ) const
	{
		return m_buffer.Get( ) ? m_buffer->SRV( ) : nullptr;
	}

	TypedBuffer( std::size_t numElement, const void* initData = nullptr ) :
		m_numElement( numElement )
	{
		InitResource( initData );
	}

	TypedBuffer( ) = default;
	~TypedBuffer( ) = default;
	TypedBuffer( const TypedBuffer& ) = default;
	TypedBuffer& operator=( const TypedBuffer& ) = default;
	TypedBuffer( TypedBuffer&& ) = default;
	TypedBuffer& operator=( TypedBuffer&& ) = default;

	operator aga::Buffer*( ) const
	{
		return m_buffer.Get( );
	}

protected:
	void InitResource( const void* initData )
	{
		if ( m_numElement > 0 )
		{
			BUFFER_TRAIT trait = {
			static_cast<UINT>( sizeof( T ) ),
			static_cast<UINT>( m_numElement ),
			RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::GPU_WRITE,
			RESOURCE_BIND_TYPE::SHADER_RESOURCE | RESOURCE_BIND_TYPE::RANDOM_ACCESS,
			RESOURCE_MISC::BUFFER_STRUCTURED };

			m_buffer = aga::Buffer::Create( trait, initData );
			if ( IsInRenderThread( ) )
			{
				m_buffer->Init( );
			}
			else
			{
				EnqueueRenderTask( [buffer = m_buffer]( )
				{
					buffer->Init( );
				} );
			}
		}
	}

private:
	RefHandle<aga::Buffer> m_buffer;
	std::size_t m_numElement = 0;
};

template <typename T>
class TypedUploadBuffer : public UploadBuffer
{
public:
	TypedUploadBuffer( std::size_t numElement, const void* initData ) : UploadBuffer( sizeof( T ), numElement, initData ) {}

	TypedUploadBuffer( ) : UploadBuffer( sizeof( T ), 0, nullptr ) {}
	~TypedUploadBuffer( ) = default;
	TypedUploadBuffer( const TypedUploadBuffer& ) = default;
	TypedUploadBuffer& operator=( const TypedUploadBuffer& ) = default;
	TypedUploadBuffer( TypedUploadBuffer&& ) = default;
	TypedUploadBuffer& operator=( TypedUploadBuffer&& ) = default;
};
