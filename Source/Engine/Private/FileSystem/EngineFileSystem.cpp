#include "stdafx.h"
#include "EngineFileSystem.h"

#include "InterfaceFactories.h"

#include <cstddef>
#include <list>

struct EngineFileSystemOverlapped : public FileSystemOverlapped
{
	EngineFileSystemOverlapped( ) : FileSystemOverlapped{ }
	{ }

	IFileSystem::IOCompletionCallback m_callback;
};

class EngineFileSystem : public IFileSystem
{
public:
	virtual FileHandle OpenFile( const char* filePath ) override;
	virtual void CloseFile( const FileHandle& handle ) override;
	virtual uint32 GetFileSize( const FileHandle& handle ) const override;
	virtual bool ReadAsync( const FileHandle& handle, char* buffer, uint32 size, IOCompletionCallback* callback = nullptr ) override;

	EngineFileSystem( );
	virtual ~EngineFileSystem( );
	EngineFileSystem( const EngineFileSystem& ) = delete;
	EngineFileSystem& operator=( const EngineFileSystem& ) = delete;
	EngineFileSystem( EngineFileSystem&& ) = delete;
	EngineFileSystem& operator=( EngineFileSystem&& ) = delete;

private:
	std::mutex m_ioRequestMutex;
	std::mutex m_callbackMutex;
	FileSystem<EngineFileSystemOverlapped> m_fileSystem;
	TaskHandle m_hWaitIO;
};

FileHandle EngineFileSystem::OpenFile( const char* filePath )
{
	return FileSystem<EngineFileSystemOverlapped>::OpenFile( filePath );
}

void EngineFileSystem::CloseFile( const FileHandle& handle )
{
	FileSystem<EngineFileSystemOverlapped>::CloseFile( handle );
}

uint32 EngineFileSystem::GetFileSize( const FileHandle& handle ) const
{
	return m_fileSystem.GetFileSize( handle );
}

bool EngineFileSystem::ReadAsync( const FileHandle& handle, char* buffer, uint32 size, IOCompletionCallback* callback )
{
	if ( handle.IsValid( ) == false )
	{
		return false;
	}

	std::lock_guard lk( m_ioRequestMutex );
	EngineFileSystemOverlapped* o = static_cast<EngineFileSystemOverlapped*>( m_fileSystem.ReadAsync( handle, buffer, size ) );
	if ( callback && ( o != nullptr ) )
	{
		o->m_callback = *callback;
	}

	return ( o != nullptr );
}

EngineFileSystem::EngineFileSystem( )
{
	auto waitIO = [this]( ) {
		while ( true )
		{
			EngineFileSystemOverlapped* o = m_fileSystem.WaitAsyncIO( );
			if ( o == nullptr )
			{
				break;
			}

			EnqueueThreadTask<ThreadType::GameThread>( [this, o]( )
			{
				if ( o->m_callback.IsBound( ) )
				{
					o->m_callback( o->m_buffer, o->m_bufferSize );
				}

				delete[] o->m_buffer;
				m_fileSystem.CleanUpIORequest( o );
			} );
		}
	};

	m_hWaitIO = EnqueueThreadTask<ThreadType::FileSystemThread>( waitIO );
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
