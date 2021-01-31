#pragma once

#include "GraphicsApiResource.h"

namespace aga
{
	class Texture;
	class Viewport;
}

namespace rendercore
{
	class Viewport
	{
	public:
		RENDERCORE_DLL DEVICE_ERROR Present( bool vSync = false );
		RENDERCORE_DLL void Clear( const float (&color)[4] );

		RENDERCORE_DLL std::pair<UINT, UINT> Size( ) const;

		aga::Texture* Texture( );

		RENDERCORE_DLL Viewport( int width, int height, HWND hWnd, RESOURCE_FORMAT format );
		RENDERCORE_DLL ~Viewport( );

	private:
		RefHandle<aga::Viewport> m_pViewport;
	};
}