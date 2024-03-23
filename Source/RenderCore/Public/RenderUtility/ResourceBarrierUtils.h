#pragma once

#include "Buffer.h"
#include "GraphicsApiResource.h"
#include "Texture.h"

namespace rendercore
{
	agl::ResourceTransition Transition( agl::Buffer& buffer, agl::ResourceState state, uint32 subResource = agl::AllSubResource );
	agl::ResourceTransition Transition( agl::Texture& texture, agl::ResourceState state, uint32 subResource = agl::AllSubResource );

	agl::UavBarrier UavBarrier( agl::Buffer& buffer );
	agl::UavBarrier UavBarrier( agl::Texture& texture );
}