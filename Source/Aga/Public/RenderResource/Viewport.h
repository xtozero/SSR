#pragma once

#include "GraphicsApiResource.h"

namespace aga
{
	class Texture;

	class Viewport : public DeviceDependantResource
	{
	public:
		virtual DEVICE_ERROR Present( bool vSync = false ) = 0;
		virtual void Clear( const float( &clearColor )[4] ) = 0;
		virtual void* Handle( ) const = 0;
		virtual std::pair<UINT, UINT> Size( ) const = 0;
		virtual void Resize( const std::pair<UINT, UINT>& newSize ) = 0;
		virtual Texture* Texture( ) = 0;
	};
}