#include "stdafx.h"
#include "Buffer.h"

#include "InterfaceFactories.h"
#include "IRenderResourceManager.h"

namespace agl
{
	RefHandle<Buffer> Buffer::Create( const BufferTrait& trait, const char* debugName )
	{
		return GetInterface<IResourceManager>()->CreateBuffer( trait, debugName, ResourceState::Common, nullptr );
	}

	RefHandle<Buffer> Buffer::Create( const BufferTrait& trait, const char* debugName, ResourceState initialState )
	{
		return GetInterface<IResourceManager>()->CreateBuffer( trait, debugName, initialState, nullptr );
	}

	RefHandle<Buffer> Buffer::Create( const BufferTrait& trait, const char* debugName, const void* initData )
	{
		return GetInterface<IResourceManager>()->CreateBuffer( trait, debugName, ResourceState::Common, initData );
	}

	RefHandle<Buffer> Buffer::Create( const BufferTrait& trait, const char* debugName, ResourceState initialState, const void* initData )
	{
		return GetInterface<IResourceManager>()->CreateBuffer( trait, debugName, initialState, initData );
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

	bool Buffer::IsDynamic() const
	{
		return HasAnyFlags( m_trait.m_access, ResourceAccessFlag::CpuRead )
			|| HasAnyFlags( m_trait.m_access, ResourceAccessFlag::CpuWrite );
	}
	
	Buffer::Buffer( ResourceState initialState ) noexcept
		: m_state( initialState )
	{
		m_isBuffer = true;
	}
}
