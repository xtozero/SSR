#include "ResourceBarrierUtils.h"

namespace rendercore
{
	agl::ResourceTransition Transition( agl::Buffer& buffer, agl::ResourceState state, uint32 subResource )
	{
		return agl::ResourceTransition
		{
			.m_pResource = buffer.Resource(),
			.m_pTransitionable = &buffer,
			.m_subResource = subResource,
			.m_state = state
		};
	}

	agl::ResourceTransition Transition( agl::Texture& texture, agl::ResourceState state, uint32 subResource )
	{
		return agl::ResourceTransition
		{
			.m_pResource = texture.Resource(),
			.m_pTransitionable = &texture,
			.m_subResource = subResource,
			.m_state = state
		};
	}

	agl::UavBarrier UavBarrier( agl::Buffer& buffer )
	{
		return agl::UavBarrier{
			.m_pResource = buffer.Resource()
		};
	}

	agl::UavBarrier UavBarrier( agl::Texture& texture )
	{
		return agl::UavBarrier{
			.m_pResource = texture.Resource()
		};
	}
}