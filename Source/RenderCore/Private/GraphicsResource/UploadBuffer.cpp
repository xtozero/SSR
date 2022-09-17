#include "stdafx.h"
#include "UploadBuffer.h"

#include "AbstractGraphicsInterface.h"
#include "TaskScheduler.h"

#include <cassert>

namespace rendercore
{
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

	agl::Buffer* UploadBuffer::Resource()
	{
		return m_buffer.Get();
	}

	const agl::Buffer* UploadBuffer::Resource() const
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
			agl::BUFFER_TRAIT trait = {
				m_elementSize,
				m_numElement,
				agl::ResourceAccessFlag::GpuRead |
				agl::ResourceAccessFlag::CpuWrite,
				agl::ResourceBindType::ShaderResource,
				agl::ResourceMisc::BufferStructured,
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

	void* UploadBuffer::LockImple()
	{
		return GraphicsInterface().Lock( m_buffer ).m_data;
	}
}
