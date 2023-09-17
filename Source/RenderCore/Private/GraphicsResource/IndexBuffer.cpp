#include "stdafx.h"
#include "IndexBuffer.h"

#include "AbstractGraphicsInterface.h"
#include "CommandList.h"
#include "TaskScheduler.h"

namespace rendercore
{
	void IndexBuffer::Resize( uint32 newNumElement, bool copyPreviousData )
	{
		if ( newNumElement > m_numElement )
		{
			IndexBuffer newBuffer( newNumElement, nullptr, m_isDWORD, m_isDynamic );

			if ( copyPreviousData )
			{
				auto commandList = GetCommandList();
				commandList.CopyResource( newBuffer.m_buffer, m_buffer, Size() );
			}

			( *this ) = std::move( newBuffer );
		}
	}

	void* IndexBuffer::Lock()
	{
		assert( IsInRenderThread() );
		return GraphicsInterface().Lock( m_buffer ).m_data;
	}

	void IndexBuffer::Unlock()
	{
		assert( IsInRenderThread() );
		GraphicsInterface().UnLock( m_buffer );
	}

	size_t IndexBuffer::Size() const
	{
		return ElementSize() * m_numElement;
	}

	size_t IndexBuffer::ElementSize() const
	{
		return m_isDWORD ? sizeof( uint32 ) : sizeof( uint16 );
	}

	agl::Buffer* IndexBuffer::Resource()
	{
		return m_buffer.Get();
	}

	const agl::Buffer* IndexBuffer::Resource() const
	{
		return m_buffer.Get();
	}

	IndexBuffer::IndexBuffer( uint32 numElement, const void* initData, bool isDWORD, bool isDynamic ) 
		: m_numElement( numElement )
		, m_isDWORD( isDWORD )
		, m_isDynamic( isDynamic )
	{
		InitResource( initData );
	}

	void IndexBuffer::InitResource( const void* initData )
	{
		agl::ResourceAccessFlag accessFlag = m_isDynamic 
			? ( agl::ResourceAccessFlag::Upload ) 
			: ( agl::ResourceAccessFlag::Default );

		agl::BufferTrait trait = {
			.m_stride = m_isDWORD ? sizeof( uint32 ) : sizeof( uint16 ),
			.m_count = m_numElement,
			.m_access = accessFlag,
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
