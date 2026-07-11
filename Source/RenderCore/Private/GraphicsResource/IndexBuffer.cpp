#include "IndexBuffer.h"

#include "AbstractGraphicsInterface.h"
#include "CommandList.h"
#include "Config/DefaultRenderCoreConfig.h"
#include "TaskScheduler.h"

namespace rendercore
{
	void IndexBuffer::Resize( uint32 newNumElement, bool copyPreviousData )
	{
		if ( newNumElement > m_numElement )
		{
			IndexBuffer newBuffer( newNumElement, agl::ResourceState::CopyDest, nullptr, m_isDWORD, m_isDynamic );

			if ( copyPreviousData )
			{
				auto commandList = GetCommandList();
				commandList.CopyResource( newBuffer.m_buffer.Get(), m_buffer.Get(), true, Size() );
			}

			( *this ) = std::move( newBuffer );
		}
	}

	void* IndexBuffer::Lock()
	{
		assert( IsInRenderThread() );
		return GraphicsInterface().Lock( m_buffer.Get() ).m_data;
	}

	void IndexBuffer::Unlock()
	{
		assert( IsInRenderThread() );
		GraphicsInterface().UnLock( m_buffer.Get() );
	}

	uint32 IndexBuffer::Size() const
	{
		return ElementSize() * m_numElement;
	}

	uint32 IndexBuffer::ElementSize() const
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

	IndexBuffer::IndexBuffer( uint32 numElement, agl::ResourceState initialState, const void* initData, bool isDWORD, bool isDynamic )
		: m_numElement( numElement )
		, m_isDWORD( isDWORD )
		, m_isDynamic( isDynamic )
	{
		InitResource( initialState, initData );
	}

	void IndexBuffer::InitResource( agl::ResourceState initialState, const void* initData )
	{
		if ( m_numElement == 0 )
		{
			return;
		}

		agl::ResourceAccess resourceAccess = m_isDynamic
			? ( agl::ResourceAccess::Upload ) 
			: ( agl::ResourceAccess::Default );

		auto bindType = agl::ResourceBindType::IndexBuffer;
		auto miscFlag = agl::ResourceMisc::None;
		if ( DefaultRenderCore::SupportsVisibilityRendering() )
		{
			bindType |= agl::ResourceBindType::ShaderResource;
		}

		agl::BufferDesc desc = {
			.m_stride = m_isDWORD ? sizeof( uint32 ) : sizeof( uint16 ),
			.m_count = m_numElement,
			.m_access = resourceAccess,
			.m_bindType = bindType,
			.m_miscFlag = miscFlag,
			.m_format = m_isDWORD ? agl::ResourceFormat::R32_UINT : agl::ResourceFormat::R16_UINT
		};

		m_buffer = agl::Buffer::Create( desc, "Index", initialState, initData );
	}
}
