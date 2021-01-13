#pragma once

#include "AssetLoader/AssetLoader.h"
#include "Core/InterfaceFactories.h"

#include <cassert>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <string>
#include <type_traits>
#include <vector>

class BinaryChunk
{
public:
	std::size_t Size( ) const { return m_size; }
	char* Data( ) { return m_data; }
	const char* Data( ) const { return m_data; }

	explicit BinaryChunk( std::size_t size )
	{
		Alloc( size );
	}

	~BinaryChunk( )
	{
		Purge( );
	}

	BinaryChunk( const BinaryChunk& other )
	{
		( *this ) = other;
	}

	BinaryChunk& operator=( const BinaryChunk& other )
	{
		if ( this != &other )
		{
			Purge( );
			Alloc( other.m_size );
			std::memcpy( m_data, other.m_data, m_size );
		}

		return *this;
	}

	BinaryChunk( BinaryChunk&& other )
	{
		( *this ) = std::move( other );
	}

	BinaryChunk& operator=( BinaryChunk&& other )
	{
		if ( this != &other )
		{
			Purge( );
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
	void Alloc( std::size_t size )
	{
		Purge( );
		m_size = size;

		if ( size > 0 )
		{
			m_data = new char[size];
		}
	}

	void Purge( )
	{
		delete[] m_data;
		m_data = nullptr;
		m_size = 0;
	}

	char* m_data = nullptr;
	std::size_t m_size = 0;
};

class Archive
{
public:
	Archive( ) = default;
	Archive( const char* begin, const char* end ) : m_curPos( begin ), m_endPos( end ) {}
	Archive( const char* begin, std::size_t size ) : m_curPos( begin ), m_endPos( begin + size ) {}
	~Archive( ) = default;
	Archive( const Archive& ) = default;
	Archive& operator=( const Archive& ) = default;
	Archive( Archive&& ) = default;
	Archive& operator=( Archive&& ) = default;

	template <typename T>
	Archive& operator<<( T& value )
	{
		if ( IsWriteMode( ) )
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
		assert( IsWriteMode( ) );
		WriteData( value );
		return *this;
	}

	std::size_t Size( ) const
	{
		return m_buffer.size( );
	}

	const char* Data( ) const
	{
		return m_buffer.data( );
	}

	void WriteToFile( const std::filesystem::path& dstFile ) const
	{
		std::ofstream outputFile( dstFile, std::ios::binary | std::ios::trunc );
		if ( outputFile.good( ) )
		{
			outputFile.write( m_buffer.data( ), m_buffer.size( ) );
		}
	}

	bool IsWriteMode( ) const
	{
		return m_curPos == nullptr;
	}

private:
	// Read
	template <typename T>
	void ReadData( T& value )
	{
		static_assert( std::is_arithmetic_v<T> || std::is_enum_v<T> );
		if ( CanRead( sizeof( T ) ) == false )
		{
			return;
		}

		const T* pValue = reinterpret_cast<const T*>( m_curPos );
		value = *pValue;
		m_curPos += sizeof( T );
	}

	template <typename T>
	void ReadData( std::shared_ptr<T>& value )
	{
		std::string path;
		ReadData( path );

		if ( path.empty( ) )
		{
			return;
		}

		IAssetLoader::LoadCompletionCallback onLoadComplete;
		onLoadComplete.BindFunctor( [&value]( const std::shared_ptr<void>& asset )
		{
			value = std::reinterpret_pointer_cast<T>( asset );
		} );

		AssetLoaderSharedHandle handle = GetInterface<IAssetLoader>( )->RequestAsyncLoad( path, onLoadComplete );

		assert( handle->IsLoadingInProgress( ) || handle->IsLoadComplete( ) );
	}

	template <typename T>
	void ReadData( T*& value )
	{
		std::string path;
		ReadData( path );

		if ( path.empty( ) )
		{
			return;
		}

		IAssetLoader::LoadCompletionCallback onLoadComplete;
		onLoadComplete.BindFunctor( [&value]( const std::shared_ptr<void>& asset )
		{
			value = std::reinterpret_pointer_cast<T*>( asset );
		} );

		AssetLoaderSharedHandle handle = GetInterface<IAssetLoader>( )->RequestAsyncLoad( path, onLoadComplete );

		assert( handle->IsLoadingInProgress( ) || handle->IsLoadComplete( ) );
	}

	void ReadData( char* str )
	{
		assert( str );
		if ( CanRead( sizeof( std::size_t ) ) == false )
		{
			return;
		}

		// Read string length
		std::size_t stringSize = 0;
		( *this ) << stringSize;

		if ( ( stringSize <= 0 ) || ( CanRead( stringSize ) == false ) )
		{
			return;
		}

		std::strcpy( str, m_curPos );
		str[stringSize] = '\0';
		m_curPos += stringSize;
	}

	void ReadData( std::string& value )
	{
		if ( CanRead( sizeof( std::size_t ) ) == false )
		{
			return;
		}

		// Read string length
		std::size_t stringSize = 0;
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
		if ( CanRead( sizeof( std::size_t ) ) == false )
		{
			return;
		}

		// Read binary data length
		std::size_t size = 0;
		( *this ) << size;

		if ( ( size <= 0 ) || ( CanRead( size ) == false ) )
		{
			return;
		}

		BinaryChunk temp( size );
		std::memcpy( temp.Data( ), m_curPos, size );
		chunk = std::move( temp );
		m_curPos += size;
	}

	bool CanRead( std::size_t size )
	{
		return ( m_curPos + size ) <= m_endPos;
	}

	// Write
	template <typename T>
	void WriteData( const T& value )
	{
		static_assert( std::is_arithmetic_v<T> || std::is_enum_v<T> );
		char* cur = (char*)( &value );

		for ( std::size_t i = 0; i < sizeof( T ); ++i )
		{
			m_buffer.push_back( cur[i] );
		}
	}

	template <typename T>
	void WriteData( const T* value )
	{
		if ( value == nullptr )
		{
			// Empty path for placeholder
			WriteData( "" );
		}
		else
		{
			WriteData( value->Path( ).generic_string( ) );
		}
	}

	template <typename T>
	void WriteData( T* value )
	{
		if ( value == nullptr )
		{
			// Empty path for placeholder
			WriteData( "" );
		}
		else
		{
			WriteData( value->Path( ).generic_string( ) );
		}
	}

	template <typename T>
	void WriteData( std::shared_ptr<T>& value )
	{
		WriteData( value.get( ) );
	}
	
	void WriteData( char* str )
	{
		WriteData( static_cast<const char*>( str ) );
	}

	void WriteData( const char* str )
	{
		std::size_t len = strlen( str );
		( *this ) << len;

		for ( std::size_t i = 0; i < len; ++i )
		{
			m_buffer.push_back( str[i] );
		}
	}

	void WriteData( const std::string& str )
	{
		WriteData( str.data( ) );
	}

	void WriteData( const std::filesystem::path& path )
	{
		WriteData( path.u8string( ) );
	}

	void WriteData( const BinaryChunk& chunk )
	{
		std::size_t size = chunk.Size( );
		( *this ) << size;

		const char* data = chunk.Data( );
		for ( std::size_t i = 0; i < size; ++i )
		{
			m_buffer.push_back( data[i] );
		}
	}

	const char* m_curPos = nullptr;
	const char* m_endPos = nullptr;
	std::vector<char> m_buffer;
};