#include "stdafx.h"

#include "Common/IAga.h"
#include "Core/InterfaceFactories.h"
#include "D3D11Api.h"

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