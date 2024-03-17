#include "ConstantBuffer.h"

#include "AbstractGraphicsInterface.h"
#include "TaskScheduler.h"

namespace rendercore
{
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

	size_t ConstantBuffer::Size() const
	{
		return m_size;
	}

	agl::Buffer* ConstantBuffer::Resource()
	{
		return m_buffer.Get();
	}

	const agl::Buffer* ConstantBuffer::Resource() const
	{
		return m_buffer.Get();
	}

	ConstantBuffer::ConstantBuffer( uint32 size )
		: m_size( size )
	{
		InitResource( size );
	}

	void ConstantBuffer::InitResource( uint32 size )
	{
		agl::BufferTrait trait = {
			.m_stride = size,
			.m_count = 1,
			.m_access = agl::ResourceAccessFlag::Upload,
			.m_bindType = agl::ResourceBindType::ConstantBuffer,
			.m_miscFlag = agl::ResourceMisc::None,
			.m_format = agl::ResourceFormat::Unknown
		};

		m_buffer = agl::Buffer::Create( trait, "Constant" );
		EnqueueRenderTask(
			[buffer = m_buffer]()
			{
				buffer->Init();
			} );
	}
}
