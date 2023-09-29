#include "stdafx.h"
#include "Viewport.h"

#include "InterfaceFactories.h"
#include "IRenderResourceManager.h"

namespace agl
{
	RefHandle<Viewport> Viewport::Create( uint32 width, uint32 height, void* hWnd, ResourceFormat format, const float4& bgColor, bool useDedicateTexture )
	{
		return GetInterface<IResourceManager>()->CreateViewport( width, height, hWnd, format, bgColor, useDedicateTexture );
	}
}