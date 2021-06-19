#pragma once

#include "GraphicsApiResource.h"

namespace aga
{
	class Texture;

	class Viewport : public DeviceDependantResource
	{
	public:
		AGA_DLL static RefHandle<Viewport> Create( int width, int height, void* hWnd, RESOURCE_FORMAT format );

		virtual DEVICE_ERROR Present( bool vSync = false ) = 0;
		virtual void Clear( const float( &clearColor )[4] ) = 0;
		virtual void Bind( ) = 0;
		virtual void* Handle( ) const = 0;
		virtual std::pair<UINT, UINT> Size( ) const = 0;
		virtual void Resize( const std::pair<UINT, UINT>& newSize ) = 0;
		virtual Texture* Texture( ) = 0;
	};
}