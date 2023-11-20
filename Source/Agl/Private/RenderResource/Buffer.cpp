#include "stdafx.h"
#include "Buffer.h"

#include "InterfaceFactories.h"
#include "IRenderResourceManager.h"

namespace agl
{
	RefHandle<Buffer> Buffer::Create( const BufferTrait& trait, const char* debugName, const void* initData )
	{
		return GetInterface<IResourceManager>()->CreateBuffer( trait, debugName, initData );
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
	
	Buffer::Buffer() noexcept
	{
		m_isBuffer = true;
	}
}
