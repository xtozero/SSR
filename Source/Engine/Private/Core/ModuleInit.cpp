#include "stdafx.h"

#include "AppConfig/AppConfig.h"
#include "AssetFactory.h"
#include "AssetLoader.h"
#include "CommandLine.h"
#include "FileSystem.h"
#include "EnumStringMap.h"
#include "IEngine.h"
#include "InterfaceFactories.h"
#include "NameTypes.h"
#include "TaskScheduler.h"
#include "TransientAllocator.h"

namespace
{
	IAppConfig* g_appConfig = nullptr;
	IAssetFactory* g_assetFactory = nullptr;
	IAssetLoader* g_assetLoader = nullptr;
	CommandLine* g_commandLine = nullptr;
	IFileSystem* g_fileSystem = nullptr;
	IEngine* g_gameEngine = nullptr;
	INamePool* g_namePool = nullptr;
	ITaskScheduler* g_taskScheduler = nullptr;
	TransientAllocators g_transientAllocators;
	
	void* GetAppConfig()
	{
		return g_appConfig;
	}

	void* GetAssetFactory()
	{
		return g_assetFactory;
	}

	void* GetAssetLoader()
	{
		return g_assetLoader;
	}

	void* GetCommandLineOption()
	{
		return g_commandLine;
	}

	void* GetFileSystem()
	{
		return g_fileSystem;
	}

	void* GetGameEngine()
	{
		return g_gameEngine;
	}

	void* GetNamePool()
	{
		return g_namePool;
	}

	void* GetTaskScheduler()
	{
		return g_taskScheduler;
	}

	void* GetTransientAllocators()
	{
		return &g_transientAllocators;
	}
}

ENGINE_FUNC_DLL void BootUpModules( )
{
	RegisterFactory<CommandLine>( &GetCommandLineOption );
	RegisterFactory<IAppConfig>( &GetAppConfig );
	RegisterFactory<IAssetFactory>( &GetAssetFactory );
	RegisterFactory<IAssetLoader>( &GetAssetLoader );
	RegisterFactory<IEngine>( &GetGameEngine );
	RegisterFactory<IEnumStringMap>( &GetEnumStringMap );
	RegisterFactory<IFileSystem>( &GetFileSystem );
	RegisterFactory<INamePool>( &GetNamePool );
	RegisterFactory<ITaskScheduler>( &GetTaskScheduler );
	RegisterFactory<TransientAllocators>( &GetTransientAllocators );

	g_taskScheduler = CreateTaskScheduler();
	g_fileSystem = CreateFileSystem();
	g_appConfig = CreateAppConfig();
	g_assetFactory = CreateAssetFactory();
	g_assetLoader = CreateAssetLoader();
	g_commandLine = CreateCommandLine();
	g_gameEngine = CreatePlatformEngine();
	g_namePool = CreateNamePool();
}

ENGINE_FUNC_DLL void ShutdownModules()
{
	DestroyAppConfig( g_appConfig );
	DestroyAssetLoader( g_assetLoader );
	DestroyAssetFactory( g_assetFactory );
	DestroyCommandLine( g_commandLine );
	DestroyFileSystem( g_fileSystem );
	DestroyPlatformEngine( g_gameEngine );
	DestroyNamePool( g_namePool );
	DestroyTaskScheduler( g_taskScheduler );

	UnregisterFactory<CommandLine>();
	UnregisterFactory<IAppConfig>();
	UnregisterFactory<IAssetFactory>();
	UnregisterFactory<IAssetLoader>();
	UnregisterFactory<IEngine>();
	UnregisterFactory<IEnumStringMap>();
	UnregisterFactory<IFileSystem>();
	UnregisterFactory<INamePool>();
	UnregisterFactory<ITaskScheduler>();
	UnregisterFactory<TransientAllocators>();
}