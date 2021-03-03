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

	class RasterizerState : public DeviceDependantResource
	{
	public:
		AGA_DLL static RefHandle<RasterizerState> Create( const RASTERIZER_STATE_TRAIT& trait );
	};
}