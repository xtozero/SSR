#include "Canvas.h"

#include "InterfaceFactories.h"
#include "IRenderResourceManager.h"

namespace agl
{
	RefHandle<Canvas> Canvas::Create( uint32 width, uint32 height, void* hWnd, ResourceFormat format, const float4& clearColor )
	{
		return GetInterface<IResourceManager>()->CreateCanvas( width, height, hWnd, format, clearColor );
	}
}
