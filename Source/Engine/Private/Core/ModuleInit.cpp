#include "AppConfig/AppConfig.h"
#include "AssetFactory.h"
#include "AssetLoader.h"
#include "CommandLine.h"
#include "CpuProfiler.h"
#include "EnumStringMap.h"
#include "FileSystem.h"
#include "IEngine.h"
#include "InterfaceFactories.h"
#include "NameTypes.h"
#include "TaskScheduler.h"
#include "TransientAllocator.h"

namespace
{
	using ::engine::CommandLine;
	using ::engine::ICpuProfiler;
	using ::engine::IEngine;

	IAppConfig* g_appConfig = nullptr;
	IAssetFactory* g_assetFactory = nullptr;
	IAssetLoader* g_assetLoader = nullptr;
	CommandLine* g_commandLine = nullptr;
	ICpuProfiler* g_cpuProfiler = nullptr;
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

	void* GetCpuProfiler()
	{
		return g_cpuProfiler;
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

namespace engine
{
	ENGINE_FUNC_DLL void BootUpModules()
	{
		RegisterFactory<CommandLine>( &GetCommandLineOption );
		RegisterFactory<IAppConfig>( &GetAppConfig );
		RegisterFactory<IAssetFactory>( &GetAssetFactory );
		RegisterFactory<IAssetLoader>( &GetAssetLoader );
		RegisterFactory<ICpuProfiler>( &GetCpuProfiler );
		RegisterFactory<IEngine>( &GetGameEngine );
		RegisterFactory<IEnumStringMap>( &GetEnumStringMap );
		RegisterFactory<IFileSystem>( &GetFileSystem );
		RegisterFactory<INamePool>( &GetNamePool );
		RegisterFactory<ITaskScheduler>( &GetTaskScheduler );
		RegisterFactory<TransientAllocators>( &GetTransientAllocators );

		g_cpuProfiler = CreateCpuProfiler();
		g_taskScheduler = CreateTaskScheduler();
		g_fileSystem = CreateFileSystem();
		g_appConfig = CreateAppConfig();
		g_assetFactory = CreateAssetFactory();
		g_assetLoader = CreateAssetLoader();
		g_commandLine = CreateCommandLine();
		g_namePool = CreateNamePool();
		g_gameEngine = CreatePlatformEngine();
	}

	ENGINE_FUNC_DLL void ShutdownModules()
	{
		DestroyAssetLoader( g_assetLoader );
		DestroyPlatformEngine( g_gameEngine );
		DestroyNamePool( g_namePool );
		DestroyCommandLine( g_commandLine );
		DestroyAssetFactory( g_assetFactory );
		DestroyAppConfig( g_appConfig );
		DestroyFileSystem( g_fileSystem );
		DestroyTaskScheduler( g_taskScheduler );
		DestroyCpuProfiler( g_cpuProfiler );

		UnregisterFactory<CommandLine>();
		UnregisterFactory<IAppConfig>();
		UnregisterFactory<IAssetFactory>();
		UnregisterFactory<IAssetLoader>();
		UnregisterFactory<ICpuProfiler>();
		UnregisterFactory<IEngine>();
		UnregisterFactory<IEnumStringMap>();
		UnregisterFactory<IFileSystem>();
		UnregisterFactory<INamePool>();
		UnregisterFactory<ITaskScheduler>();
		UnregisterFactory<TransientAllocators>();
	}
}