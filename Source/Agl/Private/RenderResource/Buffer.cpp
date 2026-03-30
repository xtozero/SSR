#include "Buffer.h"

#include "InterfaceFactories.h"
#include "IResourceManager.h"

namespace agl
{
	RefHandle<Buffer> Buffer::Create( const BufferTrait& trait, const char* debugName )
	{
		auto newBuffer = GetInterface<IResourceManager>()->CreateBuffer( trait, debugName, ResourceState::Common, nullptr );
		EnqueueRenderTask(
			[buffer = newBuffer]()
			{
				buffer->Init();
			} );

		return newBuffer;
	}

	RefHandle<Buffer> Buffer::Create( const BufferTrait& trait, const char* debugName, ResourceState initialState )
	{
		auto newBuffer = GetInterface<IResourceManager>()->CreateBuffer( trait, debugName, initialState, nullptr );
		EnqueueRenderTask(
			[buffer = newBuffer]()
			{
				buffer->Init();
			} );

		return newBuffer;
	}

	RefHandle<Buffer> Buffer::Create( const BufferTrait& trait, const char* debugName, const void* initData )
	{
		auto newBuffer = GetInterface<IResourceManager>()->CreateBuffer( trait, debugName, ResourceState::Common, initData );
		EnqueueRenderTask(
			[buffer = newBuffer]()
			{
				buffer->Init();
			} );

		return newBuffer;
	}

	RefHandle<Buffer> Buffer::Create( const BufferTrait& trait, const char* debugName, ResourceState initialState, const void* initData )
	{
		auto newBuffer = GetInterface<IResourceManager>()->CreateBuffer( trait, debugName, initialState, initData );
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

	const BufferTrait& Buffer::GetTrait() const
	{
		return m_trait;
	}

	uint32 Buffer::Stride() const
	{
		return m_trait.m_stride;
	}

	uint32 Buffer::Size() const
	{
		return m_trait.m_stride * m_trait.m_count;
	}

	bool Buffer::IsDynamic() const
	{
		return HasAnyFlags( m_trait.m_access, ResourceAccess::CpuRead )
			|| HasAnyFlags( m_trait.m_access, ResourceAccess::CpuWrite );
	}
	
	Buffer::Buffer( ResourceState initialState ) noexcept
		: m_state( initialState )
	{
		m_isBuffer = true;
	}
}
