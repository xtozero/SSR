#include "stdafx.h"
#include "IndexBuffer.h"

#include "TaskScheduler.h"

namespace rendercore
{
	aga::Buffer* IndexBuffer::Resource()
	{
		return m_buffer.Get();
	}

	const aga::Buffer* IndexBuffer::Resource() const
	{
		return m_buffer.Get();
	}

	IndexBuffer::IndexBuffer( uint32 numElement, const void* initData, bool isDWORD ) : m_numElement( numElement ), m_isDWORD( isDWORD )
	{
		InitResource( initData );
	}

	void IndexBuffer::InitResource( const void* initData )
	{
		BUFFER_TRAIT trait = {
			m_isDWORD ? sizeof( DWORD ) : sizeof( WORD ),
			m_numElement,
			RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::GPU_WRITE,
			RESOURCE_BIND_TYPE::INDEX_BUFFER,
			RESOURCE_MISC::NONE,
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
