#include "stdafx.h"
#include "Buffer.h"

#include "InterfaceFactories.h"
#include "IRenderResourceManager.h"

namespace agl
{
	RefHandle<Buffer> Buffer::Create( const BUFFER_TRAIT& trait, const void* initData )
	{
		return GetInterface<IResourceManager>()->CreateBuffer( trait, initData );
	}

	ResourceState Buffer::GetState() const
	{
		return m_state;
	}

	void Buffer::SetState( ResourceState state )
	{
		m_state = state;
	}
}
