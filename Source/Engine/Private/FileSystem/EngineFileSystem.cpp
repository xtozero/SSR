#include "stdafx.h"
#include "Core/InterfaceFactories.h"
#include "FileSystem/EngineFileSystem.h"

#include <cstddef>

class EngineFileSystem : public IFileSystem
{
public:
	virtual [[nodiscard]] FileHandle OpenFile( const char* filePath ) override;
	virtual void CloseFile( const FileHandle& handle ) override;
	virtual unsigned long GetFileSize( const FileHandle& handle ) const override;
	virtual void ReadAsync( const FileHandle& handle, char* buffer, unsigned long size ) const override;

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

FileHandle EngineFileSystem::OpenFile( const char* filePath )
{
	std::lock_guard lk( m_fileSystemMutex );
	return m_fileSystem.OpenFile( filePath );
}

void EngineFileSystem::CloseFile( const FileHandle& handle )
{
	std::lock_guard lk( m_fileSystemMutex );
	m_fileSystem.CloseFile( handle );
}

unsigned long EngineFileSystem::GetFileSize( const FileHandle& handle ) const
{
	return m_fileSystem.GetFileSize( handle );
}

void EngineFileSystem::ReadAsync( const FileHandle& handle, char* buffer, unsigned long size ) const
{ 
	m_fileSystem.ReadAsync( handle, buffer, size );
}

EngineFileSystem::EngineFileSystem( )
{
	constexpr std::size_t FileSystemWorkerAffinityMask = WorkerAffinityMask<ThreadType::FileSystemThread>( );
	m_hWaitIO = GetInterface<ITaskScheduler>()->GetTaskGroup( 1, FileSystemWorkerAffinityMask );

	class TaskWaitAsyncRead
	{
	public:
		void DoTask( )
		{
			m_fileSystem.WaitAsyncIO( );
		}

		TaskWaitAsyncRead( FileSystem& fileSystem ) : m_fileSystem( fileSystem ) {  }

	private:
		FileSystem& m_fileSystem;
	};

	GetInterface<ITaskScheduler>( )->Run( m_hWaitIO, Task<TaskWaitAsyncRead>::Create( m_fileSystem ) );
}

EngineFileSystem::~EngineFileSystem( )
{
	m_fileSystem.SuspendWaitAsyncIO( );
	GetInterface<ITaskScheduler>( )->Wait( m_hWaitIO );
}

IFileSystem* CreateFileSystem( )
{ 
	return new EngineFileSystem( );
}

void DestroyFileSystem( IFileSystem* fileSystem )
{
	delete fileSystem;
}
