#pragma once

#include "GraphicsApiResource.h"
#include "SizedTypes.h"

namespace agl
{
	class Canvas;
	class ICommandListBase;
	class Texture;

	class Viewport : public DeviceDependantResource
	{
	public:
		AGL_DLL static RefHandle<Viewport> Create( uint32 width, uint32 height, ResourceFormat format, const float4& bgColor );
		AGL_DLL static RefHandle<Viewport> Create( Canvas& canvas );

		virtual void Clear( const float( &clearColor )[4] ) = 0;
		virtual void Bind( ICommandListBase& commandList ) const = 0;
		virtual std::pair<uint32, uint32> Size() const = 0;
		virtual void Resize( const std::pair<uint32, uint32>& newSize ) = 0;
		virtual agl::Texture* Texture() = 0;
	};
}