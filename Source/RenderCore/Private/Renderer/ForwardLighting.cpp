#include "stdafx.h"
#include "Renderer\ForwardLighting.h"

#include "AbstractGraphicsInterface.h"
#include "TaskScheduler.h"

namespace rendercore
{
	void ForwardLightBuffer::Initialize( uint32 bytePerElement, uint32 numElements, agl::ResourceFormat format )
	{
		agl::BufferTrait trait = {
			.m_stride = bytePerElement,
			.m_count = numElements,
			.m_access = agl::ResourceAccessFlag::CpuWrite,
			.m_bindType = agl::ResourceBindType::ShaderResource,
			.m_miscFlag = agl::ResourceMisc::None,
			.m_format = format
		};

		m_buffer = agl::Buffer::Create( trait );
		EnqueueRenderTask(
			[buffer = m_buffer]()
			{
				buffer->Init();
			} );
	}

	void* ForwardLightBuffer::Lock()
	{
		assert( IsInRenderThread() );
		return GraphicsInterface().Lock( m_buffer ).m_data;
	}

	void ForwardLightBuffer::Unlock()
	{
		assert( IsInRenderThread() );
		GraphicsInterface().UnLock( m_buffer );
	}

	agl::ShaderResourceView* ForwardLightBuffer::SRV()
	{
		return m_buffer ? m_buffer->SRV() : nullptr;
	}

	const agl::ShaderResourceView* ForwardLightBuffer::SRV() const
	{
		return m_buffer ? m_buffer->SRV() : nullptr;
	}

	agl::Buffer* ForwardLightBuffer::Resource()
	{
		return m_buffer;
	}

	const agl::Buffer* ForwardLightBuffer::Resource() const
	{
		return m_buffer;
	}
}
