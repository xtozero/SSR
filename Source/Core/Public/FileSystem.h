#pragma once

#include "FixedBlockMemoryPool.h"

#include <cassert>
#include <cstddef>
#include <cstring>
#include <functional>
#include <thread>
#include <Windows.h>

struct FileSystemOverlapped : public OVERLAPPED
{
	FileSystemOverlapped( ) : OVERLAPPED{ }
	{ }

	bool m_isIOComplete = false;
	char* m_buffer = nullptr;
	unsigned long m_bufferSize = 0;
};

struct FileHandle
{
	void* m_handle = nullptr;

	bool IsValid( ) const
	{
		return m_handle != INVALID_HANDLE_VALUE;
	}
};

class FileSystem
{
private:
	static constexpr int MAX_FILE_PATH = 2048;
	static constexpr int MAX_CONCURRENT_FILE_READ = 128;

public:
	FileHandle OpenFile( const char* filePath )
	{
		char normalizedPath[MAX_FILE_PATH] = { '\0' };
		strncpy_s( normalizedPath, filePath, MAX_FILE_PATH );

		for ( char* c = normalizedPath; *c != '\0'; ++c )
		{
			if ( *c == '\\' )
			{
				*c = '/';
			}
		}

		HANDLE hFile = CreateFileA( normalizedPath, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, nullptr );
		if ( hFile == INVALID_HANDLE_VALUE )
		{
			return FileHandle{ INVALID_HANDLE_VALUE };
		}

		return FileHandle{ hFile };
	}

	unsigned long GetFileSize( const FileHandle& handle ) const
	{
		HANDLE hFile = handle.m_handle;

		LARGE_INTEGER fileSize;
		if ( ( GetFileSizeEx( hFile, &fileSize ) == 0 )
			|| ( fileSize.HighPart != 0 ) ) // Reading more than 4GB file is not surpported  
		{
			assert( false );
		}

		return fileSize.LowPart;
	}

	void* ReadAsync( const FileHandle& handle, char* buffer, unsigned long size )
	{
		if ( handle.IsValid( ) == false )
		{
			return false;
		}

		HANDLE hFile = handle.m_handle;
		HANDLE hPort = CreateIoCompletionPort( hFile, m_completionPort, (ULONG_PTR)size, 0 );
		if ( hPort == NULL || hPort != m_completionPort )
		{
			assert( false );
			return false;
		}

		FileSystemOverlapped* overlapped = m_overlappedPool.Allocate( 1 );
		overlapped->m_buffer = buffer;
		overlapped->m_bufferSize = size;

		if ( ::ReadFile( hFile, buffer, static_cast<DWORD>( size ), nullptr, overlapped ) == 0 )
		{
			if ( GetLastError( ) != ERROR_IO_PENDING )
			{
				m_overlappedPool.Deallocate( overlapped, 1 );
				return nullptr;
			}
		}

		return overlapped;
	}

	void CloseFile( FileHandle handle )
	{
		CloseHandle( handle.m_handle );
	}

	void WaitAsyncIO( ) const
	{ 
		DWORD numberOfBytesTransferred;
		unsigned long* requestSize;
		FileSystemOverlapped* o;

		while ( true )
		{
			if ( GetQueuedCompletionStatus( m_completionPort, &numberOfBytesTransferred, (PULONG_PTR)&requestSize, (LPOVERLAPPED*)&o, INFINITE ) )
			{
				assert( numberOfBytesTransferred == (long long)requestSize );
				if ( ( numberOfBytesTransferred == 0 )
					&& ( requestSize == nullptr )
					&& ( o == nullptr ) )
				{
					break;
				}

				if ( o )
				{
					o->m_isIOComplete = true;
				}
			}
		}
	}

	void SuspendWaitAsyncIO( )
	{
		PostQueuedCompletionStatus( m_completionPort, 0, 0, nullptr );
	}

	void CleanUpIORequest( FileSystemOverlapped* overlapped )
	{
		m_overlappedPool.Deallocate( overlapped, 1 );
	}

	FileSystem( ) : m_overlappedPool( MAX_CONCURRENT_FILE_READ )
	{
		if ( m_completionPort == INVALID_HANDLE_VALUE )
		{
			m_completionPort = CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 1 );
		}
	}
	~FileSystem( )
	{
		CloseHandle( m_completionPort );
		m_completionPort = INVALID_HANDLE_VALUE;
	}
	FileSystem( const FileSystem& ) = delete;
	FileSystem& operator=( const FileSystem& ) = delete;
	FileSystem( FileSystem&& ) = delete;
	FileSystem& operator=( FileSystem&& ) = delete;

private:
	HANDLE m_completionPort = INVALID_HANDLE_VALUE;
	FixedBlockMemoryPool<FileSystemOverlapped> m_overlappedPool;
};
