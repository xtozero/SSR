#include "stdafx.h"

#include "AssetLoader/AssetLoader.h"
#include "Core/IEngine.h"
#include "Core/InterfaceFactories.h"
#include "DataStructure/EnumStringMap.h"
#include "FileSystem/EngineFileSystem.h"
#include "MultiThread/EngineTaskScheduler.h"

namespace
{
	IEngine* g_gameEngine = nullptr;
	IFileSystem* g_fileSystem = nullptr;
	ITaskScheduler* g_taskScheduler = nullptr;
	IAssetLoader* g_assetLoader = nullptr;

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

	void* GetAssetLoader( )
	{
		return g_assetLoader;
	}
}

ENGINE_FUNC_DLL void BootUpModules( )
{
	RegisterFactory<IAssetLoader>( &GetAssetLoader );
	RegisterFactory<IEngine>( &GetGameEngine );
	RegisterFactory<IEnumStringMap>( &GetEnumStringMap );
	RegisterFactory<IFileSystem>( &GetFileSystem );
	RegisterFactory<ITaskScheduler>( &GetTaskScheduler );

	g_taskScheduler = CreateTaskScheduler( );
	g_fileSystem = CreateFileSystem( );
	g_assetLoader = CreateAssetLoader( );
	g_gameEngine = CreatePlatformEngine( );
}

ENGINE_FUNC_DLL void ShutdownModules( )
{
	DestoryAssetLoader( g_assetLoader );
	DestroyFileSystem( g_fileSystem );
	DestroyPlatformEngine( g_gameEngine );
	DestroyTaskScheduler( g_taskScheduler );

	UnregisterFactory<IAssetLoader>( );
	UnregisterFactory<IEngine>( );
	UnregisterFactory<IEnumStringMap>( );
	UnregisterFactory<IFileSystem>( );
	UnregisterFactory<ITaskScheduler>( );
}