#pragma once

#include "GraphicsApiResource.h"
#include "SizedTypes.h"

namespace agl
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
		RENDERCORE_DLL agl::DeviceError Present( bool vSync = false );
		RENDERCORE_DLL void Clear( const float (&color)[4] );
		RENDERCORE_DLL void Bind( agl::ICommandList& commandList ) const;

		RENDERCORE_DLL void* Handle( ) const;
		RENDERCORE_DLL std::pair<uint32, uint32> Size( ) const;

		RENDERCORE_DLL void Resize( const std::pair<uint32, uint32>& newSize );

		agl::Texture* Texture( );

		RENDERCORE_DLL Viewport( uint32 width, uint32 height, HWND hWnd, agl::ResourceFormat format );
		RENDERCORE_DLL ~Viewport( );

	private:
		agl::RefHandle<agl::Viewport> m_pViewport;
	};
}