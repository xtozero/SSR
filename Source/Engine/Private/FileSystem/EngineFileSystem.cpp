#include "stdafx.h"
#include "FileSystem/EngineFileSystem.h"

#include <cstddef>

EngineFileSystem* g_FileSystem = nullptr;

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
	m_hWaitIO = g_TaskScheduler->GetTaskGroup( 1 );

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

	g_TaskScheduler->Run( m_hWaitIO, Task<TaskWaitAsyncRead>::Create( m_fileSystem ) );
}

EngineFileSystem::~EngineFileSystem( )
{
	m_fileSystem.SuspendWaitAsyncIO( );
	g_TaskScheduler->Wait( m_hWaitIO );
}
