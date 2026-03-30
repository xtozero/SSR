#include "Canvas.h"

#include "InterfaceFactories.h"
#include "IResourceManager.h"

namespace agl
{
	RefHandle<Canvas> Canvas::Create( uint32 width, uint32 height, void* hWnd, ResourceFormat format, const float4& clearColor )
	{
		auto newCanvas = GetInterface<IResourceManager>()->CreateCanvas( width, height, hWnd, format, clearColor );
		EnqueueRenderTask(
			[canvas = newCanvas]()
			{
				canvas->Init();
			} );

		return newCanvas;
	}
}
