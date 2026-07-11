#include "Viewport.h"

#include "InterfaceFactories.h"
#include "IResourceManager.h"

namespace agl
{
	RefHandle<Viewport> Viewport::Create( uint32 width, uint32 height, ResourceFormat format, const float4& bgColor )
	{
		RefHandle<Viewport> newViewport = GetInterface<IResourceManager>()->CreateViewport( width, height, format, bgColor );
		EnqueueRenderTask(
			[viewport = newViewport]()
			{
				viewport->Init();
			} );

		return newViewport;
	}

	RefHandle<Viewport> Viewport::Create( Canvas& canvas )
	{
		RefHandle<Viewport> newViewport = GetInterface<IResourceManager>()->CreateViewport( canvas );
		EnqueueRenderTask(
			[viewport = newViewport]()
			{
				viewport->Init();
			} );

		return newViewport;
	}
}