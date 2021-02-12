#pragma once
#include "common.h"
#include "GraphicsApiResource.h"

namespace aga
{
	class Buffer : public DeviceDependantResource
	{
	public:
		AGA_DLL static RefHandle<Buffer> Create( const BUFFER_TRAIT& trait, const void* initData = nullptr );
	};
}