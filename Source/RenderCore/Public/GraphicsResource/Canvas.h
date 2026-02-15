#pragma once

#include "ColorTypes.h"
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

		RENDERCORE_DLL agl::DeviceError Present( bool vSync = false, bool allowTearing = true );
		RENDERCORE_DLL void Clear();

		RENDERCORE_DLL void* Handle() const;
		RENDERCORE_DLL std::pair<uint32, uint32> Size() const;
		RENDERCORE_DLL void Resize( uint32 width, uint32 height );

		RENDERCORE_DLL agl::Texture* Texture();

		RENDERCORE_DLL Canvas() = default;
		RENDERCORE_DLL Canvas( uint32 width, uint32 height, void* hWnd, agl::ResourceFormat format, const float4& clearColor );
		RENDERCORE_DLL ~Canvas();

	private:
		RefHandle<agl::Canvas> m_canvas;
	};
}
