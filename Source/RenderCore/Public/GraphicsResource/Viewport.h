#pragma once

#include "GraphicsApiResource.h"
#include "SizedTypes.h"

namespace agl
{
	class ICommandListBase;
	class Texture;
	class Viewport;
}

namespace rendercore
{
	class Viewport
	{
	public:
		RENDERCORE_DLL void OnBeginFrameRendering();
		RENDERCORE_DLL void OnEndFrameRendering();

		RENDERCORE_DLL agl::DeviceError Present( bool vSync = false );
		RENDERCORE_DLL void Clear( const float( &color )[4] );
		RENDERCORE_DLL void Bind( agl::ICommandListBase& commandList ) const;

		RENDERCORE_DLL void* Handle() const;
		RENDERCORE_DLL std::pair<uint32, uint32> Size() const;

		RENDERCORE_DLL void Resize( const std::pair<uint32, uint32>& newSize );

		RENDERCORE_DLL agl::Texture* Texture();
		RENDERCORE_DLL agl::Texture* Canvas();

		RENDERCORE_DLL Viewport( uint32 width, uint32 height, HWND hWnd, agl::ResourceFormat format, const float4& bgColor, bool useDedicateTexture );
		RENDERCORE_DLL ~Viewport();

	private:
		agl::RefHandle<agl::Viewport> m_pViewport;
	};
}