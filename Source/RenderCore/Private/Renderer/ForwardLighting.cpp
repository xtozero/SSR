#include "Renderer/ForwardLighting.h"

#include "AbstractGraphicsInterface.h"
#include "TaskScheduler.h"

namespace rendercore
{
	void ForwardLightBuffer::Initialize( uint32 bytePerElement, uint32 numElements, agl::ResourceFormat format )
	{
		uint32 newSize = bytePerElement * numElements;
		uint32 curSize = 0;
		if ( m_buffer.Get() )
		{
			const agl::BufferDesc& desc = m_buffer->GetDesc();
			curSize = desc.m_count * desc.m_stride;
		}

		if ( newSize > curSize )
		{
			agl::BufferDesc desc = {
				.m_stride = bytePerElement,
				.m_count = numElements,
				.m_access = agl::ResourceAccess::Upload,
				.m_bindType = agl::ResourceBindType::ShaderResource,
				.m_miscFlag = agl::ResourceMisc::None,
				.m_format = format
			};

			m_buffer = agl::Buffer::Create( desc, "ForwardLight" );
		}
	}

	void* ForwardLightBuffer::Lock()
	{
		assert( IsInRenderThread() );
		return GraphicsInterface().Lock( m_buffer.Get() ).m_data;
	}

	void ForwardLightBuffer::Unlock()
	{
		assert( IsInRenderThread() );
		GraphicsInterface().UnLock( m_buffer.Get() );
	}

	agl::ShaderResourceView* ForwardLightBuffer::SRV()
	{
		return m_buffer.Get() ? m_buffer->SRV() : nullptr;
	}

	const agl::ShaderResourceView* ForwardLightBuffer::SRV() const
	{
		return m_buffer.Get() ? m_buffer->SRV() : nullptr;
	}

	agl::Buffer* ForwardLightBuffer::Resource()
	{
		return m_buffer.Get();
	}

	const agl::Buffer* ForwardLightBuffer::Resource() const
	{
		return m_buffer.Get();
	}

	ForwardLightingResource::ForwardLightingResource()
	{
		m_shaderArguments = ForwardLightParameters::CreateShaderArguments();
	}
}
