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
	class Canvas;

	class Viewport
	{
	public:
		RENDERCORE_DLL void Clear( const float( &color )[4] );
		RENDERCORE_DLL void Bind( agl::ICommandListBase& commandList ) const;

		RENDERCORE_DLL std::pair<uint32, uint32> Size() const;
		RENDERCORE_DLL std::pair<uint32, uint32> SizeOnRenderThread() const;

		RENDERCORE_DLL void Resize( const std::pair<uint32, uint32>& newSize );

		RENDERCORE_DLL agl::Texture* Texture();

		RENDERCORE_DLL Viewport( uint32 width, uint32 height, agl::ResourceFormat format, const float4& bgColor );
		RENDERCORE_DLL explicit Viewport( rendercore::Canvas& canvas );
		RENDERCORE_DLL ~Viewport();

	private:
		agl::RefHandle<agl::Viewport> m_pViewport;
	};
}