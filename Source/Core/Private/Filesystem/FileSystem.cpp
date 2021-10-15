#include "FileSystem.h"

#include "InterfaceFactories.h"

#include <cstddef>
#include <list>

struct FileSystemImplOverlapped : public FileSystemOverlapped
{
	FileSystemImplOverlapped( ) : FileSystemOverlapped{ }
	{ }

	IFileSystem::IOCompletionCallback m_callback;
};

class FileSystemImpl : public IFileSystem
{
public:
	virtual FileHandle OpenFile( const char* filePath ) override;
	virtual void CloseFile( const FileHandle& handle ) override;
	virtual uint32 GetFileSize( const FileHandle& handle ) const override;
	virtual bool ReadAsync( const FileHandle& handle, char* buffer, uint32 size, IOCompletionCallback* callback = nullptr ) override;

	FileSystemImpl( );
	virtual ~FileSystemImpl( );
	FileSystemImpl( const FileSystemImpl& ) = delete;
	FileSystemImpl& operator=( const FileSystemImpl& ) = delete;
	FileSystemImpl( FileSystemImpl&& ) = delete;
	FileSystemImpl& operator=( FileSystemImpl&& ) = delete;

private:
	std::mutex m_ioRequestMutex;
	FileSystem<FileSystemImplOverlapped> m_fileSystem;
	TaskHandle m_hWaitIO;
};

FileHandle FileSystemImpl::OpenFile( const char* filePath )
{
	return FileSystem<FileSystemImplOverlapped>::OpenFile( filePath );
}

void FileSystemImpl::CloseFile( const FileHandle& handle )
{
	FileSystem<FileSystemImplOverlapped>::CloseFile( handle );
}

uint32 FileSystemImpl::GetFileSize( const FileHandle& handle ) const
{
	return m_fileSystem.GetFileSize( handle );
}

bool FileSystemImpl::ReadAsync( const FileHandle& handle, char* buffer, uint32 size, IOCompletionCallback* callback )
{
	if ( handle.IsValid( ) == false )
	{
		return false;
	}

	std::lock_guard lk( m_ioRequestMutex );
	FileSystemImplOverlapped* o = static_cast<FileSystemImplOverlapped*>( m_fileSystem.ReadAsync( handle, buffer, size ) );
	if ( callback && ( o != nullptr ) )
	{
		o->m_callback = *callback;
	}

	return ( o != nullptr );
}

FileSystemImpl::FileSystemImpl( )
{
	auto waitIO = [this]( ) {
		while ( true )
		{
			FileSystemImplOverlapped* o = m_fileSystem.WaitAsyncIO( );
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

FileSystemImpl::~FileSystemImpl( )
{
	m_fileSystem.SuspendWaitAsyncIO( );
	GetInterface<ITaskScheduler>( )->Wait( m_hWaitIO );
}

IFileSystem* CreateFileSystem( )
{ 
	return new FileSystemImpl( );
}

void DestroyFileSystem( IFileSystem* fileSystem )
{
	delete fileSystem;
}
