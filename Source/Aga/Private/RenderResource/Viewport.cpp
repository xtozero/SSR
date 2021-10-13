#include "stdafx.h"
#include "Viewport.h"

#include "InterfaceFactories.h"
#include "IRenderResourceManager.h"

namespace aga
{
	RefHandle<Viewport> Viewport::Create( uint32 width, uint32 height, void* hWnd, RESOURCE_FORMAT format )
	{
		return GetInterface<IResourceManager>( )->CreateViewport( width, height, hWnd, format );
	}
}