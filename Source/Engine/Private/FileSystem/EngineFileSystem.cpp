#include "stdafx.h"
#include "Core/InterfaceFactories.h"
#include "FileSystem/EngineFileSystem.h"

#include <cstddef>
#include <list>

class EngineFileSystem : public IFileSystem
{
public:
	virtual [[nodiscard]] FileHandle OpenFile( const char* filePath ) override;
	virtual void CloseFile( const FileHandle& handle ) override;
	virtual unsigned long GetFileSize( const FileHandle& handle ) const override;
	virtual bool ReadAsync( const FileHandle& handle, char* buffer, unsigned long size, IOCompletionCallback* callback = nullptr ) override;
	virtual void DispatchCallback( ) override;

	EngineFileSystem( );
	virtual ~EngineFileSystem( );
	EngineFileSystem( const EngineFileSystem& ) = delete;
	EngineFileSystem& operator=( const EngineFileSystem& ) = delete;
	EngineFileSystem( EngineFileSystem&& ) = delete;
	EngineFileSystem& operator=( EngineFileSystem&& ) = delete;

private:
	std::mutex m_ioRequestMutex;
	std::mutex m_callbackMutex;
	FileSystem m_fileSystem;
	GroupHandle m_hWaitIO;

	using CallBack = std::pair<FileSystemOverlapped*, Delegate<void, char*, unsigned long>>;
	std::list<CallBack> m_callbacks;
};

FileHandle EngineFileSystem::OpenFile( const char* filePath )
{
	return FileSystem::OpenFile( filePath );
}

void EngineFileSystem::CloseFile( const FileHandle& handle )
{
	FileSystem::CloseFile( handle );
}

unsigned long EngineFileSystem::GetFileSize( const FileHandle& handle ) const
{
	return m_fileSystem.GetFileSize( handle );
}

bool EngineFileSystem::ReadAsync( const FileHandle& handle, char* buffer, unsigned long size, IOCompletionCallback* callback )
{
	if ( handle.IsValid( ) == false )
	{
		return false;
	}

	std::lock_guard lk( m_ioRequestMutex );
	FileSystemOverlapped* o = static_cast<FileSystemOverlapped*>( m_fileSystem.ReadAsync( handle, buffer, size ) );
	if ( o != nullptr )
	{
		m_callbacks.emplace_back( o, callback ? *callback : IOCompletionCallback( ) );
	}

	return ( o != nullptr );
}

void EngineFileSystem::DispatchCallback( )
{
	if ( m_callbacks.size( ) == 0 )
	{
		return;
	}

	std::lock_guard lk( m_callbackMutex );
	for ( auto iter = m_callbacks.begin( ); iter != m_callbacks.end( ); )
	{
		CallBack& callback = *iter;
		FileSystemOverlapped* o = callback.first;
		if ( o->m_isIOComplete )
		{
			if ( callback.second.IsBound( ) )
			{
				callback.second( o->m_buffer, o->m_bufferSize );
			}

			iter = m_callbacks.erase( iter );
		}
		else
		{
			++iter;
		}

		m_fileSystem.CleanUpIORequest( o );
	}
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
