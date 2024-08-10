#pragma once

#include "GraphicsApiResource.h"
#include "RefHandle.h"
#include "SizedTypes.h"

namespace agl
{
	class Texture;

	class Canvas : public GraphicsApiResource
	{
	public:
		AGL_DLL static RefHandle<Canvas> Create( uint32 width, uint32 height, void* hWnd, ResourceFormat format );

		virtual void OnBeginFrameRendering() = 0;
		virtual void OnEndFrameRendering() = 0;

		virtual DeviceError Present( bool vSync = false ) = 0;
		virtual void Clear( const float( &clearColor )[4] ) = 0;

		virtual void* Handle() const = 0;
		virtual std::pair<uint32, uint32> Size() const = 0;
		virtual void Resize( const std::pair<uint32, uint32>& newSize ) = 0;

		virtual agl::Texture* Texture() = 0;
	};
}