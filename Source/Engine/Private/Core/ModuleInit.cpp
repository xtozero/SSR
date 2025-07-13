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

	IAppConfig* GAppConfig = nullptr;
	IAssetFactory* GAssetFactory = nullptr;
	IAssetLoader* GAssetLoader = nullptr;
	CommandLine* GCommandLine = nullptr;
	ICpuProfiler* GCpuProfiler = nullptr;
	IFileSystem* GFileSystem = nullptr;
	IEngine* GEngine = nullptr;
	INamePool* GNamePool = nullptr;
	ITaskScheduler* GTaskScheduler = nullptr;
	TransientAllocators GTransientAllocators;
	
	void* GetAppConfig()
	{
		return GAppConfig;
	}

	void* GetAssetFactory()
	{
		return GAssetFactory;
	}

	void* GetAssetLoader()
	{
		return GAssetLoader;
	}

	void* GetCommandLineOption()
	{
		return GCommandLine;
	}

	void* GetCpuProfiler()
	{
		return GCpuProfiler;
	}

	void* GetFileSystem()
	{
		return GFileSystem;
	}

	void* GetGameEngine()
	{
		return GEngine;
	}

	void* GetNamePool()
	{
		return GNamePool;
	}

	void* GetTaskScheduler()
	{
		return GTaskScheduler;
	}

	void* GetTransientAllocators()
	{
		return &GTransientAllocators;
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

		GCpuProfiler = CreateCpuProfiler();
		GTaskScheduler = CreateTaskScheduler();
		GFileSystem = CreateFileSystem();
		GAppConfig = CreateAppConfig();
		GAssetFactory = CreateAssetFactory();
		GAssetLoader = CreateAssetLoader();
		GCommandLine = CreateCommandLine();
		GNamePool = CreateNamePool();
		GEngine = CreatePlatformEngine();
	}

	ENGINE_FUNC_DLL void ShutdownModules()
	{
		DestroyAssetLoader( GAssetLoader );
		DestroyPlatformEngine( GEngine );
		DestroyNamePool( GNamePool );
		DestroyCommandLine( GCommandLine );
		DestroyAssetFactory( GAssetFactory );
		DestroyAppConfig( GAppConfig );
		DestroyFileSystem( GFileSystem );
		DestroyTaskScheduler( GTaskScheduler );
		DestroyCpuProfiler( GCpuProfiler );

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