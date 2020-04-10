#include "stdafx.h"

#include "Core/IEngine.h"
#include "Core/InterfaceFactories.h"
#include "FileSystem/EngineFileSystem.h"
#include "MultiThread/EngineTaskScheduler.h"

namespace
{
	IEngine* g_gameEngine = nullptr;
	IFileSystem* g_fileSystem = nullptr;
	ITaskScheduler* g_taskScheduler = nullptr;

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
	RegisterFactory<IFileSystem>( &GetFileSystem );
	RegisterFactory<IEngine>( &GetGameEngine );
	RegisterFactory<ITaskScheduler>( &GetTaskScheduler );

	g_taskScheduler = CreateTaskScheduler( );
	g_fileSystem = CreateFileSystem( );
	g_gameEngine = CreatePlatformEngine( );
}

ENGINE_FUNC_DLL void ShutdownModules( )
{
	DestroyPlatformEngine( g_gameEngine );
	DestroyFileSystem( g_fileSystem );
	DestroyTaskScheduler( g_taskScheduler );

	UnregisterFactory<IFileSystem>( );
	UnregisterFactory<IEngine>( );
	UnregisterFactory<ITaskScheduler>( );
}