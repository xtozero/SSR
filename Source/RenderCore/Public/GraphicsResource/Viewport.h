#pragma once

#include "GraphicsApiResource.h"
#include "SizedTypes.h"

namespace aga
{
	class ICommandList;
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
		RENDERCORE_DLL void Bind( aga::ICommandList& commandList ) const;

		RENDERCORE_DLL void* Handle( ) const;
		RENDERCORE_DLL std::pair<uint32, uint32> Size( ) const;

		RENDERCORE_DLL void Resize( const std::pair<uint32, uint32>& newSize );

		aga::Texture* Texture( );

		RENDERCORE_DLL Viewport( uint32 width, uint32 height, HWND hWnd, RESOURCE_FORMAT format );
		RENDERCORE_DLL ~Viewport( );

	private:
		RefHandle<aga::Viewport> m_pViewport;
	};
}