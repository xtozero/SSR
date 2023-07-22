#include "stdafx.h"
#include "VertexBuffer.h"

#include "AbstractGraphicsInterface.h"
#include "CommandList.h"
#include "TaskScheduler.h"

namespace rendercore
{
	void VertexBuffer::Resize( uint32 newNumElement, bool copyPreviousData )
	{
		if ( newNumElement > m_numElement )
		{
			VertexBuffer newBuffer( m_elementSize, newNumElement, nullptr, m_isDynamic );

			if ( copyPreviousData )
			{
				auto commandList = GetCommandList();
				commandList.CopyResource( newBuffer.m_buffer, m_buffer, Size() );
			}

			( *this ) = std::move( newBuffer );
		}
	}

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

	size_t VertexBuffer::Size() const
	{
		return m_elementSize * m_numElement;
	}

	size_t VertexBuffer::ElementSize() const
	{
		return m_elementSize;
	}

	agl::Buffer* VertexBuffer::Resource()
	{
		return m_buffer.Get();
	}

	const agl::Buffer* VertexBuffer::Resource() const
	{
		return m_buffer.Get();
	}

	VertexBuffer::VertexBuffer( uint32 elementSize, uint32 numElement, const void* initData, bool isDynamic ) 
		: m_elementSize( elementSize )
		, m_numElement( numElement )
		, m_isDynamic( isDynamic )
	{
		InitResource( elementSize, numElement, initData );
	}

	void VertexBuffer::InitResource( uint32 elementSize, uint32 numElement, const void* initData )
	{
		agl::ResourceAccessFlag accessFlag = agl::ResourceAccessFlag::None;
		if ( m_isDynamic )
		{
			accessFlag = agl::ResourceAccessFlag::GpuRead | agl::ResourceAccessFlag::CpuWrite;
		}
		else
		{
			accessFlag = agl::ResourceAccessFlag::GpuRead | agl::ResourceAccessFlag::GpuWrite;
		}

		agl::BufferTrait trait = {
			.m_stride = elementSize,
			.m_count = numElement,
			.m_access = accessFlag,
			.m_bindType = agl::ResourceBindType::VertexBuffer,
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
