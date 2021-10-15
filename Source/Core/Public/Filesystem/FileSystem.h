#pragma once

#include "common.h"
#include "Delegate.h"
#include "FileSystemCore.h"
#include "SizedTypes.h"
#include "TaskScheduler.h"

#include <mutex>

class IFileSystem
{
public:
	using IOCompletionCallback = Delegate<void, char*&, uint32>;

	virtual FileHandle OpenFile( const char* filePath ) = 0;
	virtual void CloseFile( const FileHandle& handle ) = 0;
	virtual uint32 GetFileSize( const FileHandle& handle ) const = 0;
	virtual bool ReadAsync( const FileHandle& handle, char* buffer, uint32 size, IOCompletionCallback* callback = nullptr ) = 0;

	virtual ~IFileSystem( ) = default;
};

IFileSystem* CreateFileSystem( );
void DestroyFileSystem( IFileSystem* fileSystem );