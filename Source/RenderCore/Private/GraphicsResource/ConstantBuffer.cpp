#include "stdafx.h"
#include "ConstantBuffer.h"

#include "Core/InterfaceFactories.h"
#include "IAga.h"
#include "MultiThread/EngineTaskScheduler.h"

void ConstantBuffer::Update( const void* data, std::size_t size )
{
	assert( IsInRenderThread( ) );

	assert( data != nullptr );
	void* dst = GetInterface<aga::IAga>( )->Lock( m_buffer );
	if ( dst )
	{
		std::memcpy( dst, data, size );
	}
	GetInterface<aga::IAga>( )->UnLock( m_buffer );
}

void* ConstantBuffer::Lock( )
{
	assert( IsInRenderThread( ) );
	return GetInterface<aga::IAga>( )->Lock( m_buffer );
}

void ConstantBuffer::Unlock( )
{
	assert( IsInRenderThread( ) );
	GetInterface<aga::IAga>( )->UnLock( m_buffer );
}

void ConstantBuffer::Bind( SHADER_TYPE shaderType, UINT slot )
{
	aga::Buffer* buffer[] = { m_buffer };
	GetInterface<aga::IAga>( )->BindConstantBuffer( shaderType, slot, 1, buffer );
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
