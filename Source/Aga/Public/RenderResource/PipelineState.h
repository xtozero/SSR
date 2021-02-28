#pragma once

#include "common.h"
#include "GraphicsApiResource.h"

namespace aga
{
	class DepthStencilState : public DeviceDependantResource
	{
	public:
		AGA_DLL static RefHandle<DepthStencilState> Create( const DEPTH_STENCIL_STATE_TRAIT& trait );
	};
}