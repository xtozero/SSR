#include "stdafx.h"
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

	agl::Buffer* ConstantBuffer::Resource()
	{
		return m_buffer.Get();
	}

	const agl::Buffer* ConstantBuffer::Resource() const
	{
		return m_buffer.Get();
	}

	ConstantBuffer::ConstantBuffer( uint32 size )
	{
		InitResource( size );
	}

	void ConstantBuffer::InitResource( uint32 size )
	{
		agl::BufferTrait trait = {
			size,
			1,
			agl::ResourceAccessFlag::CpuWrite,
			agl::ResourceBindType::ConstantBuffer,
			agl::ResourceMisc::None,
			agl::ResourceFormat::Unknown
		};

		m_buffer = agl::Buffer::Create( trait );
		EnqueueRenderTask(
			[buffer = m_buffer]()
			{
				buffer->Init();
			} );
	}
}
