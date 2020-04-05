#pragma once

#include "FileSystem.h"

#include "common.h"
#include "MultiThread/EngineTaskScheduler.h"

#include <mutex>

class IFileSystem
{
public:
	virtual [[nodiscard]] FileHandle OpenFile( const char* filePath ) = 0;
	virtual void CloseFile( const FileHandle& handle ) = 0;
	virtual unsigned long GetFileSize( const FileHandle& handle ) const = 0;
	virtual void ReadAsync( const FileHandle& handle, char* buffer, unsigned long size ) const = 0;

	virtual ~IFileSystem( ) = default;
};

IFileSystem* CreateFileSystem( );
void DestroyFileSystem( IFileSystem* fileSystem );