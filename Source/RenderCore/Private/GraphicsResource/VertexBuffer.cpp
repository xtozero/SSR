#include "stdafx.h"
#include "VertexBuffer.h"

#include "AbstractGraphicsInterface.h"
#include "TaskScheduler.h"

void* VertexBuffer::Lock( )
{
	assert( IsInRenderThread( ) );
	return GraphicsInterface( ).Lock( m_buffer ).m_data;
}

void VertexBuffer::Unlock( )
{
	assert( IsInRenderThread( ) );
	GraphicsInterface( ).UnLock( m_buffer );
}

aga::Buffer* VertexBuffer::Resource( )
{
	return m_buffer.Get( );
}

const aga::Buffer* VertexBuffer::Resource( ) const
{
	return m_buffer.Get( );
}

VertexBuffer::VertexBuffer( uint32 elementSize, uint32 numElement, const void* initData, bool isDynamic ) : m_size( elementSize * numElement ), m_isDynamic( isDynamic )
{
	InitResource( elementSize, numElement, initData );
}

void VertexBuffer::InitResource( uint32 elementSize, uint32 numElement, const void* initData )
{
	uint32 accessFlag = 0;
	if ( m_isDynamic )
	{
		accessFlag = RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::CPU_WRITE;
	}
	else
	{
		accessFlag = RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::GPU_WRITE;
	}

	BUFFER_TRAIT trait = {
		elementSize,
		numElement,
		accessFlag,
		RESOURCE_BIND_TYPE::VERTEX_BUFFER,
		0,
		RESOURCE_FORMAT::UNKNOWN
	};

	m_buffer = aga::Buffer::Create( trait, initData );
	EnqueueRenderTask( [buffer = m_buffer]( )
	{
		buffer->Init( );
	} );
}
