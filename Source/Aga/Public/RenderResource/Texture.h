#pragma once
#include "common.h"
#include "GraphicsApiResource.h"

#include <utility>

namespace aga
{
	class Texture : public DeviceDependantResource
	{
	public:
		AGA_DLL static RefHandle<Texture> Create( const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData = nullptr );

		virtual std::pair<UINT, UINT> Size( ) const = 0;
	};
}
