#include "stdafx.h"
#include "ConstantBuffer.h"

#include "AbstractGraphicsInterface.h"
#include "TaskScheduler.h"

void ConstantBuffer::Update( const void* data, uint32 size )
{
	assert( IsInRenderThread() );

	assert( data != nullptr );
	void* dst = GraphicsInterface().Lock( m_buffer ).m_data;
	if ( dst )
	{
		std::memcpy( dst, data, size );
	}
	GraphicsInterface().UnLock( m_buffer );
}

void* ConstantBuffer::Lock()
{
	assert( IsInRenderThread() );
	return  GraphicsInterface().Lock( m_buffer ).m_data;
}

void ConstantBuffer::Unlock()
{
	assert( IsInRenderThread() );
	GraphicsInterface().UnLock( m_buffer );
}

aga::Buffer* ConstantBuffer::Resource()
{
	return m_buffer.Get();
}

const aga::Buffer* ConstantBuffer::Resource() const
{
	return m_buffer.Get();
}

ConstantBuffer::ConstantBuffer( uint32 size )
{
	InitResource( size );
}

void ConstantBuffer::InitResource( uint32 size )
{
	BUFFER_TRAIT trait = {
		size,
		1,
		RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::CPU_WRITE,
		RESOURCE_BIND_TYPE::CONSTANT_BUFFER,
		RESOURCE_MISC::NONE,
		RESOURCE_FORMAT::UNKNOWN
	};

	m_buffer = aga::Buffer::Create( trait );
	EnqueueRenderTask(
		[buffer = m_buffer]()
		{
			buffer->Init();
		} );
}
