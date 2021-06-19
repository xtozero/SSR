#include "stdafx.h"
#include "Viewport.h"

#include "Core/InterfaceFactories.h"
#include "IRenderResourceManager.h"

namespace aga
{
	RefHandle<Viewport> Viewport::Create( int width, int height, void* hWnd, RESOURCE_FORMAT format )
	{
		return GetInterface<IResourceManager>( )->CreateViewport( width, height, hWnd, format );
	}
}