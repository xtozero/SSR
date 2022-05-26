#include "stdafx.h"
#include "UploadBuffer.h"

#include "AbstractGraphicsInterface.h"
#include "TaskScheduler.h"

#include <cassert>

void UploadBuffer::Resize( uint32 numElement, const void* initData )
{
	bool increasedElements = numElement > m_numElement;

	if ( increasedElements )
	{
		m_numElement = numElement;
		InitResource( initData );
	}
}

void UploadBuffer::Unlock()
{
	GraphicsInterface().UnLock( m_buffer );
}

aga::Buffer* UploadBuffer::Resource()
{
	return m_buffer.Get();
}

const aga::Buffer* UploadBuffer::Resource() const
{
	return m_buffer.Get();
}

UploadBuffer::UploadBuffer( uint32 elementSize, uint32 numElement, const void* initData ) :
	m_elementSize( elementSize ), m_numElement( numElement )
{
	InitResource( initData );
}

void UploadBuffer::InitResource( const void* initData )
{
	if ( m_elementSize > 0 && m_numElement > 0 )
	{
		BUFFER_TRAIT trait = {
			m_elementSize,
			m_numElement,
			RESOURCE_ACCESS_FLAG::GPU_READ |
			RESOURCE_ACCESS_FLAG::CPU_WRITE,
			RESOURCE_BIND_TYPE::SHADER_RESOURCE,
			RESOURCE_MISC::BUFFER_STRUCTURED,
			RESOURCE_FORMAT::UNKNOWN
		};

		m_buffer = aga::Buffer::Create( trait, initData );
		EnqueueRenderTask(
			[buffer = m_buffer]()
			{
				buffer->Init();
			} );
	}
}

void* UploadBuffer::LockImple()
{
	return GraphicsInterface().Lock( m_buffer ).m_data;
}
