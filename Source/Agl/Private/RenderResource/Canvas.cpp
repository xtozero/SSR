#include "Canvas.h"

#include "InterfaceFactories.h"
#include "IResourceManager.h"

namespace agl
{
	RefHandle<Canvas> Canvas::Create( uint32 width, uint32 height, const engine::PlatformWindowContext& windowCtx, ResourceFormat format, const float4& clearColor )
	{
		RefHandle<Canvas> newCanvas = GetInterface<IResourceManager>()->CreateCanvas( width, height, windowCtx, format, clearColor );
		EnqueueRenderTask(
			[canvas = newCanvas]()
			{
				canvas->Init();
			} );

		return newCanvas;
	}
}
