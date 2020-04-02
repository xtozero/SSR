#pragma once

#include "FileSystem.h"

#include "common.h"
#include "MultiThread/EngineTaskScheduler.h"

#include <mutex>

class EngineFileSystem
{
public:
	ENGINE_DLL [[nodiscard]] FileHandle OpenFile( const char* filePath );
	ENGINE_DLL void CloseFile( const FileHandle& handle );
	ENGINE_DLL unsigned long GetFileSize( const FileHandle& handle ) const;
	ENGINE_DLL void ReadAsync( const FileHandle& handle, char* buffer, unsigned long size ) const;

	EngineFileSystem( );
	~EngineFileSystem( );
	EngineFileSystem( const EngineFileSystem& ) = delete;
	EngineFileSystem& operator=( const EngineFileSystem& ) = delete;
	EngineFileSystem( EngineFileSystem&& ) = delete;
	EngineFileSystem& operator=( EngineFileSystem&& ) = delete;

private:
	std::mutex m_fileSystemMutex;
	FileSystem m_fileSystem;
	GroupHandle m_hWaitIO;
};

extern ENGINE_DLL EngineFileSystem* g_FileSystem;