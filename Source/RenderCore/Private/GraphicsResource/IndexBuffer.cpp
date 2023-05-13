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
			.m_stride = m_isDWORD ? sizeof( uint32 ) : sizeof( uint16 ),
			.m_count = m_numElement,
			.m_access = agl::ResourceAccessFlag::GpuRead | agl::ResourceAccessFlag::GpuWrite,
			.m_bindType = agl::ResourceBindType::IndexBuffer,
			.m_miscFlag = agl::ResourceMisc::None,
			.m_format = agl::ResourceFormat::Unknown
		};

		m_buffer = agl::Buffer::Create( trait, initData );
		EnqueueRenderTask(
			[buffer = m_buffer]()
			{
				buffer->Init();
			} );
	}
}
