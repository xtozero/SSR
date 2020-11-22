#include "stdafx.h"

#include "AssetLoader/AssetLoader.h"
#include "Core/GameLogic.h"
#include "Core/InterfaceFactories.h"

namespace
{
	ILogic* g_gameLogic = nullptr;

	void* GetGameLogic( )
	{
		return g_gameLogic;
	}

	Owner<IAssetLoader*> g_renderOptionManager = nullptr;

	void* GetRenderOptionManager( )
	{
		return g_renderOptionManager;
	}
}

LOGIC_FUNC_DLL void BootUpModules( )
{
	RegisterFactory<ILogic>( &GetGameLogic );
	RegisterFactory<IAssetLoader>( &GetRenderOptionManager );

	g_gameLogic = CreateGameLogic( );
	g_renderOptionManager = CreateAssetLoader( );
}

LOGIC_FUNC_DLL void ShutdownModules( )
{
	DestoryAssetLoader( g_renderOptionManager );
	DestroyGameLogic( g_gameLogic );

	UnregisterFactory<IAssetLoader>( );
	UnregisterFactory<ILogic>( );
}