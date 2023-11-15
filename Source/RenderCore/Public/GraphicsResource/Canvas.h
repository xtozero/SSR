#pragma once

#include "GraphicsApiResource.h"

namespace agl
{
	class Canvas;
	class Texture;
}

namespace rendercore
{
	class Canvas final
	{
	public:
		agl::Canvas* Resource();

		void OnBeginFrameRendering();
		void OnEndFrameRendering();

		RENDERCORE_DLL agl::DeviceError Present( bool vSync = false );
		RENDERCORE_DLL void Clear( const float( &color )[4] );

		RENDERCORE_DLL void* Handle() const;
		RENDERCORE_DLL std::pair<uint32, uint32> Size() const;
		RENDERCORE_DLL void Resize( const std::pair<uint32, uint32>& newSize );

		RENDERCORE_DLL agl::Texture* Texture();

		RENDERCORE_DLL Canvas( uint32 width, uint32 height, void* hWnd, agl::ResourceFormat format );
		RENDERCORE_DLL ~Canvas();

	private:
		agl::RefHandle<agl::Canvas> m_canvas;
	};
}
