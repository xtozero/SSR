#pragma once

#include "FileSystem.h"

#include "common.h"
#include "Delegate.h"
#include "MultiThread/EngineTaskScheduler.h"

#include <mutex>

class IFileSystem
{
public:
	using IOCompletionCallback = Delegate<void, char*, unsigned long>;

	virtual [[nodiscard]] FileHandle OpenFile( const char* filePath ) = 0;
	virtual void CloseFile( const FileHandle& handle ) = 0;
	virtual unsigned long GetFileSize( const FileHandle& handle ) const = 0;
	virtual bool ReadAsync( const FileHandle& handle, char* buffer, unsigned long size, IOCompletionCallback* callback = nullptr ) = 0;
	virtual void DispatchCallback( ) = 0;

	virtual ~IFileSystem( ) = default;
};

IFileSystem* CreateFileSystem( );
void DestroyFileSystem( IFileSystem* fileSystem );