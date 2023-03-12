#include "stdafx.h"
#include "IndexBuffer.h"

#include "TaskScheduler.h"

namespace rendercore
{
	agl::Buffer* IndexBuffer::Resource()
	{
		return m_buffer.Get();
	}

	const agl::Buffer* IndexBuffer::Resource() const
	{
		return m_buffer.Get();
	}

	IndexBuffer::IndexBuffer( uint32 numElement, const void* initData, bool isDWORD ) : m_numElement( numElement ), m_isDWORD( isDWORD )
	{
		InitResource( initData );
	}

	void IndexBuffer::InitResource( const void* initData )
	{
		agl::BufferTrait trait = {
			m_isDWORD ? sizeof( uint32 ) : sizeof( uint16 ),
			m_numElement,
			agl::ResourceAccessFlag::GpuRead | agl::ResourceAccessFlag::GpuWrite,
			agl::ResourceBindType::IndexBuffer,
			agl::ResourceMisc::None,
			agl::ResourceFormat::Unknown
		};

		m_buffer = agl::Buffer::Create( trait, initData );
		EnqueueRenderTask(
			[buffer = m_buffer]()
			{
				buffer->Init();
			} );
	}
}
