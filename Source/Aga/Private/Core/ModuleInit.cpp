#include "stdafx.h"

#include "Core/InterfaceFactories.h"
#include "D3D11/Direct3D11.h"
#include "Render/IRenderer.h"

RENDERCORE_FUNC_DLL void BootUpModules( )
{
	RegisterFactory<IAga>( &GetD3D11GraphicsApi );

	CreateAbstractGraphicsApi( );
}

RENDERCORE_FUNC_DLL void ShutdownModules( )
{
	DestoryAbstractGraphicsApi( );

	UnregisterFactory<IAga>( );
}