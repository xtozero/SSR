#include "stdafx.h"

#include "Core/GameLogic.h"
#include "Core/InterfaceFactories.h"

namespace
{
	ILogic* g_gameLogic = nullptr;

	void* GetGameLogic( )
	{
		return g_gameLogic;
	}
}

LOGIC_FUNC_DLL void BootUpModule( )
{
	RegisterFactory<ILogic>( &GetGameLogic );

	g_gameLogic = CreateGameLogic( );
}

LOGIC_FUNC_DLL void ShutdownModules( )
{
	DestroyGameLogic( g_gameLogic );

	UnregisterFactory<ILogic>( );
}