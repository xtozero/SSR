#pragma once

#include "GraphicsApiResource.h"

namespace aga
{
	class Viewport : public DeviceDependantResource
	{
	public:
		virtual DEVICE_ERROR Present( bool vSync = false ) = 0;
		virtual void Clear( const float( &clearColor )[4] ) = 0;
		virtual std::pair<UINT, UINT> Size( ) const = 0;
	};
}