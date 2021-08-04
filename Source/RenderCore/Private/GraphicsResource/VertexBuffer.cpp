#include "stdafx.h"
#include "VertexBuffer.h"

#include "AbstractGraphicsInterface.h"
#include "MultiThread/EngineTaskScheduler.h"

void* VertexBuffer::Lock( )
{
	assert( IsInRenderThread( ) );
	return GraphicsInterface( ).Lock( m_buffer );
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

VertexBuffer::VertexBuffer( std::size_t elementSize, std::size_t numElement, const void* initData, bool isDynamic ) : m_size( elementSize * numElement ), m_isDynamic( isDynamic )
{
	InitResource( elementSize, numElement, initData );
}

void VertexBuffer::InitResource( std::size_t elementSize, std::size_t numElement, const void* initData )
{
	UINT accessFlag = 0;
	if ( m_isDynamic )
	{
		accessFlag = RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::CPU_WRITE;
	}
	else
	{
		accessFlag = RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::GPU_WRITE;
	}

	BUFFER_TRAIT trait = {
		static_cast<UINT>( elementSize ),
		static_cast<UINT>( numElement ),
		accessFlag,
		RESOURCE_BIND_TYPE::VERTEX_BUFFER,
		0,
		RESOURCE_FORMAT::UNKNOWN
	};

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
