#include "Viewport.h"

#include "InterfaceFactories.h"
#include "IRenderResourceManager.h"

namespace agl
{
	RefHandle<Viewport> Viewport::Create( uint32 width, uint32 height, ResourceFormat format, const float4& bgColor )
	{
		return GetInterface<IResourceManager>()->CreateViewport( width, height, format, bgColor );
	}

	RefHandle<Viewport> Viewport::Create( Canvas& canvas )
	{
		return GetInterface<IResourceManager>()->CreateViewport( canvas );
	}
}