#pragma once

#include "IAssetLoader.h"
#include "InterfaceFactories.h"
#include "SizedTypes.h"
#include "TaskScheduler.h"

#include <cassert>
#include <cstddef>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

class BinaryChunk
{
public:
	uint32 Size() const { return m_size; }
	char* Data() { return m_data; }
	const char* Data() const { return m_data; }

	BinaryChunk() = default;

	explicit BinaryChunk( uint32 size )
	{
		Alloc( size );
	}

	~BinaryChunk()
	{
		Purge();
	}

	BinaryChunk( const BinaryChunk& other )
	{
		( *this ) = other;
	}

	BinaryChunk& operator=( const BinaryChunk& other )
	{
		if ( this != &other )
		{
			Purge();
			Alloc( other.m_size );
			std::memcpy( m_data, other.m_data, m_size );
		}

		return *this;
	}

	BinaryChunk( BinaryChunk&& other ) noexcept
	{
		( *this ) = std::move( other );
	}

	BinaryChunk& operator=( BinaryChunk&& other ) noexcept
	{
		if ( this != &other )
		{
			Purge();
			m_data = other.m_data;
			m_size = other.m_size;
			other.m_data = nullptr;
			other.m_size = 0;
		}

		return *this;
	}

	friend bool operator==( const BinaryChunk& lhs, const BinaryChunk& rhs )
	{
		if ( lhs.m_size == rhs.m_size )
		{
			return std::memcmp( lhs.m_data, rhs.m_data, lhs.m_size ) == 0;
		}

		return false;
	}

private:
	void Alloc( uint32 size )
	{
		Purge();
		m_size = size;

		if ( size > 0 )
		{
			m_data = new char[size];
		}
	}

	void Purge()
	{
		delete[] m_data;
		m_data = nullptr;
		m_size = 0;
	}

	char* m_data = nullptr;
	uint32 m_size = 0;
};

class Archive
{
public:
	Archive() = default;
	Archive( const char* begin, const char* end ) : m_curPos( begin ), m_endPos( end ) {}
	Archive( const char* begin, size_t size ) : m_curPos( begin ), m_endPos( begin + size ) {}
	~Archive() = default;
	Archive( const Archive& ) = default;
	Archive& operator=( const Archive& ) = default;
	Archive( Archive&& ) = default;
	Archive& operator=( Archive&& ) = default;

	template <typename T>
	Archive& operator<<( T& value )
	{
		if ( IsWriteMode() )
		{
			WriteData( value );
		}
		else
		{
			ReadData( value );
		}

		return *this;
	}

	template <typename T>
	Archive& operator<<( const T& value )
	{
		assert( IsWriteMode() );
		WriteData( value );
		return *this;
	}

	void WriteRaw( const void* value, size_t size )
	{
		size_t oldSize = m_buffer.size();
		m_buffer.resize( oldSize + size );

		auto begin = static_cast<const char*>( value );
		auto end = begin + size;
		std::copy( begin, end, &m_buffer[oldSize] );
	}

	size_t Size() const
	{
		return m_buffer.size();
	}

	const char* Data() const
	{
		return m_buffer.data();
	}

	void WriteToFile( const std::filesystem::path& dstFile ) const
	{
		std::ofstream outputFile( dstFile, std::ios::binary | std::ios::trunc );
		if ( outputFile.good() )
		{
			outputFile.write( m_buffer.data(), m_buffer.size() );
		}
	}

	bool IsWriteMode() const
	{
		return m_curPos == nullptr;
	}

private:
	// Read
	template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T> || std::is_enum_v<T>>>
	void ReadData( T& value )
	{
		if ( CanRead( sizeof( T ) ) == false )
		{
			return;
		}

		const T* pValue = reinterpret_cast<const T*>( m_curPos );
		value = *pValue;
		m_curPos += sizeof( T );
	}

	template <typename AssetType, typename T>
	void RequestLoadAsset( std::string&& path, T& value )
	{
		auto assetLoader = GetInterface<IAssetLoader>();
		auto subSequentHandle = assetLoader->HandleInProcess();
		subSequentHandle->IncreasePrerequisite();

		EnqueueThreadTask<ThreadType::GameThread>(
			[&value, subSequentHandle, assetPath = std::move( path )]()
			{
				subSequentHandle->DecreasePrerequisite();

				IAssetLoader::LoadCompletionCallback onLoadComplete;
				onLoadComplete.BindFunctor( [&value, subSequentHandle]( const std::shared_ptr<void>& asset )
					{
						value = std::reinterpret_pointer_cast<AssetType>( asset );
					} );

				auto assetLoader = GetInterface<IAssetLoader>();
				assetLoader->SetHandleInProcess( subSequentHandle );
				AssetLoaderSharedHandle handle = assetLoader->RequestAsyncLoad( assetPath, onLoadComplete );
				assetLoader->SetHandleInProcess( nullptr );

				assert( handle->IsLoadingInProgress() || handle->IsLoadComplete() );
			} );
	}

	template <typename T>
	void ReadData( std::shared_ptr<T>& value )
	{
		std::string path;
		ReadData( path );

		if ( path.empty() )
		{
			return;
		}

		RequestLoadAsset<T>( std::move( path ), value );
	}

	template <typename T>
	void ReadData( std::unique_ptr<T>& value )
	{
		uint8 nullFlag = 0;
		ReadData( nullFlag );

		if ( nullFlag == 0 )
		{
			uint32 assetID = 0;
			ReadData( assetID );

			value.reset( Cast<T>( GetInterface<class IAssetFactory>()->CreateAsset( assetID ) ) );
			value->Serialize( *this );
		}
	}

	template <int32 Len>
	void ReadData( char( &str )[Len] )
	{
		if ( CanRead( sizeof( uint32 ) ) == false )
		{
			return;
		}

		// Read string length
		uint32 stringSize = 0;
		( *this ) << stringSize;

		if ( ( stringSize <= 0 ) || ( CanRead( stringSize ) == false ) )
		{
			return;
		}

#if _WIN32
		strcpy_s( str, Len, m_curPos );
#else
		std::strcpy( str, m_curPos );
#endif
		str[stringSize] = '\0';
		m_curPos += stringSize;
	}

	void ReadData( std::string& value )
	{
		if ( CanRead( sizeof( uint32 ) ) == false )
		{
			return;
		}

		// Read string length
		uint32 stringSize = 0;
		( *this ) << stringSize;

		if ( ( stringSize <= 0 ) || ( CanRead( stringSize ) == false ) )
		{
			return;
		}

		value.assign( m_curPos, stringSize );
		m_curPos += stringSize;
	}

	void ReadData( std::filesystem::path& value )
	{
		std::string pathString;
		ReadData( pathString );
		value = std::move( pathString );
	}

	void ReadData( BinaryChunk& chunk )
	{
		if ( CanRead( sizeof( uint32 ) ) == false )
		{
			return;
		}

		// Read binary data length
		uint32 size = 0;
		( *this ) << size;

		if ( ( size == 0 ) || ( CanRead( size ) == false ) )
		{
			return;
		}

		BinaryChunk temp( size );
		std::memcpy( temp.Data(), m_curPos, size );
		chunk = std::move( temp );
		m_curPos += size;
	}

	bool CanRead( size_t size )
	{
		return ( m_curPos + size ) <= m_endPos;
	}

	// Write
	template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T> || std::is_enum_v<T>>>
	void WriteData( const T& value )
	{
		char* cur = (char*)( &value );

		for ( size_t i = 0; i < sizeof( T ); ++i )
		{
			m_buffer.push_back( cur[i] );
		}
	}

	template <typename T>
	void WriteData( const std::shared_ptr<T>& value )
	{
		if ( value == nullptr )
		{
			// Empty path for placeholder
			WriteData( "" );
		}
		else
		{
			WriteData( value->Path().generic_string() );
		}
	}

	template <typename T>
	void WriteData( const std::unique_ptr<T>& value )
	{
		if ( value == nullptr )
		{
			WriteData( static_cast<uint8>( 1 ) );
		}
		else
		{
			WriteData( static_cast<uint8>( 0 ) );
			value->Serialize( *this );
		}
	}

	void WriteData( char* str )
	{
		WriteData( static_cast<const char*>( str ) );
	}

	void WriteData( const char* str, uint32 len = 0 )
	{
		if ( len == 0 )
		{
			len = static_cast<uint32>( std::strlen( str ) );
		}
		( *this ) << len;

		for ( uint32 i = 0; i < len; ++i )
		{
			m_buffer.push_back( str[i] );
		}
	}

	void WriteData( const std::string& str )
	{
		WriteData( str.data(), static_cast<uint32>( str.length() ) );
	}

	void WriteData( const std::string_view& str )
	{
		WriteData( str.data(), static_cast<uint32>( str.length() ) );
	}

	void WriteData( const std::filesystem::path& path )
	{
		WriteData( path.generic_string() );
	}

	void WriteData( const BinaryChunk& chunk )
	{
		uint32 size = chunk.Size();
		( *this ) << size;

		const char* data = chunk.Data();
		for ( uint32 i = 0; i < size; ++i )
		{
			m_buffer.push_back( data[i] );
		}
	}

	const char* m_curPos = nullptr;
	const char* m_endPos = nullptr;
	std::vector<char> m_buffer;
};