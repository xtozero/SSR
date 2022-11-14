#include "stdafx.h"
#include "VertexBuffer.h"

#include "AbstractGraphicsInterface.h"
#include "TaskScheduler.h"

namespace rendercore
{
	void* VertexBuffer::Lock()
	{
		assert( IsInRenderThread() );
		return GraphicsInterface().Lock( m_buffer ).m_data;
	}

	void VertexBuffer::Unlock()
	{
		assert( IsInRenderThread() );
		GraphicsInterface().UnLock( m_buffer );
	}

	agl::Buffer* VertexBuffer::Resource()
	{
		return m_buffer.Get();
	}

	const agl::Buffer* VertexBuffer::Resource() const
	{
		return m_buffer.Get();
	}

	VertexBuffer::VertexBuffer( uint32 elementSize, uint32 numElement, const void* initData, bool isDynamic ) : m_size( elementSize* numElement ), m_isDynamic( isDynamic )
	{
		InitResource( elementSize, numElement, initData );
	}

	void VertexBuffer::InitResource( uint32 elementSize, uint32 numElement, const void* initData )
	{
		agl::ResourceAccessFlag accessFlag = agl::ResourceAccessFlag::None;
		if ( m_isDynamic )
		{
			accessFlag = agl::ResourceAccessFlag::CpuWrite;
		}
		else
		{
			accessFlag = agl::ResourceAccessFlag::GpuRead | agl::ResourceAccessFlag::GpuWrite;
		}

		agl::BUFFER_TRAIT trait = {
			elementSize,
			numElement,
			accessFlag,
			agl::ResourceBindType::VertexBuffer,
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
