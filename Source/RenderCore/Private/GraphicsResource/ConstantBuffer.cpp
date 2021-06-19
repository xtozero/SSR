#include "stdafx.h"
#include "ConstantBuffer.h"

#include "AbstractGraphicsInterface.h"
#include "MultiThread/EngineTaskScheduler.h"

void ConstantBuffer::Update( const void* data, std::size_t size )
{
	assert( IsInRenderThread( ) );

	assert( data != nullptr );
	void* dst = GraphicsInterface( ).Lock( m_buffer );
	if ( dst )
	{
		std::memcpy( dst, data, size );
	}
	GraphicsInterface( ).UnLock( m_buffer );
}

void* ConstantBuffer::Lock( )
{
	assert( IsInRenderThread( ) );
	return  GraphicsInterface( ).Lock( m_buffer );
}

void ConstantBuffer::Unlock( )
{
	assert( IsInRenderThread( ) );
	GraphicsInterface( ).UnLock( m_buffer );
}

aga::Buffer* ConstantBuffer::Resource( )
{
	return m_buffer.Get( );
}

const aga::Buffer* ConstantBuffer::Resource( ) const
{
	return m_buffer.Get( );
}

ConstantBuffer::ConstantBuffer( std::size_t size )
{
	InitResource( size );
}

void ConstantBuffer::InitResource( std::size_t size )
{
	BUFFER_TRAIT trait = {
		static_cast<UINT>( size ),
		1,
		RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::CPU_WRITE,
		RESOURCE_BIND_TYPE::CONSTANT_BUFFER
	};

	m_buffer = aga::Buffer::Create( trait );
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

void ConstantBuffer::BindImple( VertexShader& shader, UINT slot )
{
	aga::Buffer* buffers[] = { m_buffer };
	GraphicsInterface( ).BindConstant( shader, slot, 1, buffers );
}

void ConstantBuffer::BindImple( PixelShader& shader, UINT slot )
{
	aga::Buffer* buffers[] = { m_buffer };
	GraphicsInterface( ).BindConstant( shader, slot, 1, buffers );
}

void ConstantBuffer::BindImple( ComputeShader& shader, UINT slot )
{
	aga::Buffer* buffers[] = { m_buffer };
	GraphicsInterface( ).BindConstant( shader, slot, 1, buffers );
}
