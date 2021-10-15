#include "stdafx.h"

#include "AssetFactory.h"
#include "AssetLoader.h"
#include "FileSystem.h"
#include "EnumStringMap.h"
#include "IEngine.h"
#include "InterfaceFactories.h"
#include "TaskScheduler.h"

namespace
{
	IAssetFactory* g_assetFactory = nullptr;
	IAssetLoader* g_assetLoader = nullptr;
	IEngine* g_gameEngine = nullptr;
	IFileSystem* g_fileSystem = nullptr;
	ITaskScheduler* g_taskScheduler = nullptr;
	
	void* GetAssetFactory( )
	{
		return g_assetFactory;
	}

	void* GetAssetLoader( )
	{
		return g_assetLoader;
	}

	void* GetFileSystem( )
	{
		return g_fileSystem;
	}

	void* GetGameEngine( )
	{
		return g_gameEngine;
	}

	void* GetTaskScheduler( )
	{
		return g_taskScheduler;
	}
}

ENGINE_FUNC_DLL void BootUpModules( )
{
	RegisterFactory<IAssetFactory>( &GetAssetFactory );
	RegisterFactory<IAssetLoader>( &GetAssetLoader );
	RegisterFactory<IEngine>( &GetGameEngine );
	RegisterFactory<IEnumStringMap>( &GetEnumStringMap );
	RegisterFactory<IFileSystem>( &GetFileSystem );
	RegisterFactory<ITaskScheduler>( &GetTaskScheduler );

	g_taskScheduler = CreateTaskScheduler( );
	g_fileSystem = CreateFileSystem( );
	g_assetFactory = CreateAssetFactory( );
	g_assetLoader = CreateAssetLoader( );
	g_gameEngine = CreatePlatformEngine( );
}

ENGINE_FUNC_DLL void ShutdownModules( )
{
	DestoryAssetLoader( g_assetLoader );
	DestoryAssetFactory( g_assetFactory );
	DestroyFileSystem( g_fileSystem );
	DestroyPlatformEngine( g_gameEngine );
	DestroyTaskScheduler( g_taskScheduler );

	UnregisterFactory<IAssetFactory>( );
	UnregisterFactory<IAssetLoader>( );
	UnregisterFactory<IEngine>( );
	UnregisterFactory<IEnumStringMap>( );
	UnregisterFactory<IFileSystem>( );
	UnregisterFactory<ITaskScheduler>( );
}