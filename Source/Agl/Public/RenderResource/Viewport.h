#pragma once

#include "GraphicsApiResource.h"
#include "SizedTypes.h"

namespace agl
{
	class ICommandList;
	class Texture;

	class Viewport : public DeviceDependantResource
	{
	public:
		AGL_DLL static RefHandle<Viewport> Create( uint32 width, uint32 height, void* hWnd, ResourceFormat format );

		virtual void OnBeginFrameRendering() = 0;
		virtual void OnEndFrameRendering() = 0;
		virtual DeviceError Present( bool vSync = false ) = 0;
		virtual void Clear( const float( &clearColor )[4] ) = 0;
		virtual void Bind( ICommandList& commandList ) const = 0;
		virtual void* Handle( ) const = 0;
		virtual std::pair<uint32, uint32> Size( ) const = 0;
		virtual void Resize( const std::pair<uint32, uint32>& newSize ) = 0;
		virtual Texture* Texture( ) = 0;
	};
}