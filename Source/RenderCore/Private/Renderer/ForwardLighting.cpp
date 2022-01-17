#include "stdafx.h"
#include "Renderer\ForwardLighting.h"

#include "AbstractGraphicsInterface.h"
#include "TaskScheduler.h"

void ForwardLightBuffer::Initialize( uint32 bytePerElement, uint32 numElements, RESOURCE_FORMAT format )
{
	BUFFER_TRAIT trait = {
		bytePerElement,
		numElements,
		RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::CPU_WRITE,
		RESOURCE_BIND_TYPE::SHADER_RESOURCE,
		0,
		format
	};

	m_buffer = aga::Buffer::Create( trait );
	EnqueueRenderTask( [buffer = m_buffer]( )
	{
		buffer->Init( );
	} );
}

void* ForwardLightBuffer::Lock( )
{
	assert( IsInRenderThread( ) );
	return GraphicsInterface( ).Lock( m_buffer ).m_data;
}

void ForwardLightBuffer::Unlock( )
{
	assert( IsInRenderThread( ) );
	GraphicsInterface( ).UnLock( m_buffer );
}

aga::ShaderResourceView* ForwardLightBuffer::SRV( )
{
	return m_buffer ? m_buffer->SRV( ) : nullptr;
}

const aga::ShaderResourceView* ForwardLightBuffer::SRV( ) const
{
	return m_buffer ? m_buffer->SRV( ) : nullptr;
}

aga::Buffer* ForwardLightBuffer::Resource( )
{
	return m_buffer;
}

const aga::Buffer* ForwardLightBuffer::Resource( ) const
{
	return m_buffer;
}
