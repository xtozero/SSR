#pragma once

#include "Buffer.h"
#include "GraphicsApiResource.h"
#include "Texture.h"

namespace rendercore
{
	inline agl::ResourceTransition Transition( agl::Buffer& buffer, agl::ResourceState state, uint32 subResource = agl::AllSubResource )
	{
		return agl::ResourceTransition
		{
			.m_pResource = buffer.Resource(),
			.m_pTransitionable = &buffer,
			.m_subResource = subResource,
			.m_state = state
		};
	}

	inline agl::ResourceTransition Transition( agl::Texture& texture, agl::ResourceState state, uint32 subResource = agl::AllSubResource )
	{
		return agl::ResourceTransition
		{
			.m_pResource = texture.Resource(),
			.m_pTransitionable = &texture,
			.m_subResource = subResource,
			.m_state = state
		};
	}
}