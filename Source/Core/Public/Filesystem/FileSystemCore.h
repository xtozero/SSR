#pragma once

#include "FixedBlockMemoryPool.h"
#include "SizedTypes.h"

#include <cassert>
#include <cstddef>
#include <cstring>
#include <functional>
#include <thread>
#include <Windows.h>

struct FileSystemOverlapped : public OVERLAPPED
{
	FileSystemOverlapped() : OVERLAPPED{ }
	{ }

	bool m_isIOComplete = false;
	char* m_buffer = nullptr;
	uint32 m_bufferSize = 0;
};

struct [[nodiscard]] FileHandle
{
	void* m_handle = nullptr;

	bool IsValid() const
	{
		return m_handle != INVALID_HANDLE_VALUE;
	}
};

template <typename OverlappedType>
class FileSystem
{
private:
	static constexpr uint32 MAX_FILE_PATH = 2048;
	static constexpr uint32 MAX_CONCURRENT_FILE_READ = 128;

public:
	static FileHandle OpenFile( const char* filePath )
	{
		char normalizedPath[MAX_FILE_PATH] = { '\0' };
#ifdef _MSC_VER
		strncpy_s( normalizedPath, filePath, MAX_FILE_PATH );
#else
		std::strncpy( normalizedPath, filePath, MAX_FILE_PATH );
#endif

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

	uint32 GetFileSize( const FileHandle& handle ) const
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

	void* ReadAsync( const FileHandle& handle, char* buffer, uint32 size )
	{
		if ( handle.IsValid() == false )
		{
			return nullptr;
		}

		HANDLE hFile = handle.m_handle;
		HANDLE hPort = CreateIoCompletionPort( hFile, m_completionPort, (uptrint)size, 0 );
		if ( hPort == nullptr || hPort != m_completionPort )
		{
			assert( false );
			return nullptr;
		}

		OverlappedType* overlapped = m_overlappedPool.Allocate();
		std::construct_at( overlapped );

		overlapped->m_buffer = buffer;
		overlapped->m_bufferSize = size;

		if ( ::ReadFile( hFile, buffer, size, nullptr, overlapped ) == 0 )
		{
			if ( GetLastError() != ERROR_IO_PENDING )
			{
				CleanUpIORequest( overlapped );
				return nullptr;
			}
		}

		return overlapped;
	}

	static void CloseFile( FileHandle handle )
	{
		CloseHandle( handle.m_handle );
	}

	OverlappedType* WaitAsyncIO() const
	{
		DWORD numberOfBytesTransferred = 0;
		uptrint* requestSize = nullptr;
		OverlappedType* o = nullptr;

		if ( GetQueuedCompletionStatus( m_completionPort, &numberOfBytesTransferred, (uptrint*)&requestSize, (LPOVERLAPPED*)&o, INFINITE ) )
		{
			assert( numberOfBytesTransferred == (uptrint)requestSize );
			if ( ( numberOfBytesTransferred == 0 )
				&& ( requestSize == nullptr )
				&& ( o == nullptr ) )
			{
				return nullptr;
			}

			if ( o )
			{
				o->m_isIOComplete = true;
			}

			return o;
		}

		return nullptr;
	}

	void SuspendWaitAsyncIO()
	{
		PostQueuedCompletionStatus( m_completionPort, 0, 0, nullptr );
	}

	void CleanUpIORequest( OverlappedType* overlapped )
	{
		std::destroy_at( overlapped );
		m_overlappedPool.Deallocate( overlapped );
	}

	FileSystem()
	{
		if ( m_completionPort == INVALID_HANDLE_VALUE )
		{
			m_completionPort = CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 1 );
		}
	}
	~FileSystem()
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
	FixedBlockMemoryPool<OverlappedType> m_overlappedPool;
};
