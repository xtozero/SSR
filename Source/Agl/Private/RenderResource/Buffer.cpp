#include "Buffer.h"

#include "InterfaceFactories.h"
#include "IResourceManager.h"

namespace agl
{
	RefHandle<Buffer> Buffer::Create( const BufferDesc& desc, const char* debugName )
	{
		RefHandle<Buffer> newBuffer = GetInterface<IResourceManager>()->CreateBuffer( desc, debugName, ResourceState::Common, nullptr );
		EnqueueRenderTask(
			[buffer = newBuffer]()
			{
				buffer->Init();
			} );

		return newBuffer;
	}

	RefHandle<Buffer> Buffer::Create( const BufferDesc& desc, const char* debugName, ResourceState initialState )
	{
		RefHandle<Buffer> newBuffer = GetInterface<IResourceManager>()->CreateBuffer( desc, debugName, initialState, nullptr );
		EnqueueRenderTask(
			[buffer = newBuffer]()
			{
				buffer->Init();
			} );

		return newBuffer;
	}

	RefHandle<Buffer> Buffer::Create( const BufferDesc& desc, const char* debugName, const void* initData )
	{
		RefHandle<Buffer> newBuffer = GetInterface<IResourceManager>()->CreateBuffer( desc, debugName, ResourceState::Common, initData );
		EnqueueRenderTask(
			[buffer = newBuffer]()
			{
				buffer->Init();
			} );

		return newBuffer;
	}

	RefHandle<Buffer> Buffer::Create( const BufferDesc& desc, const char* debugName, ResourceState initialState, const void* initData )
	{
		RefHandle<Buffer> newBuffer = GetInterface<IResourceManager>()->CreateBuffer( desc, debugName, initialState, initData );
		EnqueueRenderTask(
			[buffer = newBuffer]()
			{
				buffer->Init();
			} );

		return newBuffer;
	}

	ResourceState Buffer::GetResourceState() const
	{
		return m_state;
	}

	void Buffer::SetResourceState( ResourceState state )
	{
		m_state = state;
	}

	const BufferDesc& Buffer::GetDesc() const
	{
		return m_desc;
	}

	uint32 Buffer::Stride() const
	{
		return m_desc.m_stride;
	}

	uint32 Buffer::Size() const
	{
		return m_desc.m_stride * m_desc.m_count;
	}

	bool Buffer::IsDynamic() const
	{
		return HasAnyFlags( m_desc.m_access, ResourceAccess::CpuRead )
			|| HasAnyFlags( m_desc.m_access, ResourceAccess::CpuWrite );
	}
	
	Buffer::Buffer( ResourceState initialState ) noexcept
		: m_state( initialState )
	{
		m_isBuffer = true;
	}
}
