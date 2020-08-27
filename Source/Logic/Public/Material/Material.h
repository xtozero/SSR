#pragma once

#include "Math/CXMFloat.h"

#include <map>
#include <string>

class MaterialPropertyStorage
{
public:
	void* GetRaw( ) const
	{
		return m_memory;
	}

	void* Allocate( std::size_t size )
	{
		if ( m_size < size )
		{
			std::free( m_memory );
			m_memory = std::malloc( size );
			m_size = size;
		}

		return m_memory;
	}

	void Deallocate( )
	{
		std::free( m_memory );
		m_memory = nullptr;
		m_size = 0;
	}

	MaterialPropertyStorage( ) = default;
	~MaterialPropertyStorage( ) = default;
	MaterialPropertyStorage( const MaterialPropertyStorage& ) = delete;
	MaterialPropertyStorage& operator=( const MaterialPropertyStorage& ) = delete;
	MaterialPropertyStorage( MaterialPropertyStorage&& );
	MaterialPropertyStorage& operator=( MaterialPropertyStorage&& );

private:
	void* m_memory = nullptr;
	std::size_t m_size = 0;
};

inline void* operator new( std::size_t count, MaterialPropertyStorage& storage )
{
	return storage.Allocate( count );
}

inline void operator delete( void* pMem, [[maybe_unused]] MaterialPropertyStorage& storage )
{
	std::free( pMem );
}

class MaterialProperty
{
public:
	void Clone( MaterialPropertyStorage& storage ) const;
	int AsInteger( ) const;
	float AsFloat( ) const;
	const CXMFLOAT4& AsVector( ) const;
	const char* AsString( ) const;

	MaterialProperty( ) = default;
	~MaterialProperty( );
	MaterialProperty( const MaterialProperty& other );
	MaterialProperty& operator=( const MaterialProperty& other );
	MaterialProperty( MaterialProperty&& other );
	MaterialProperty& operator=( MaterialProperty&& other );

	explicit MaterialProperty( int value );
	MaterialProperty& operator=( int value );
	explicit MaterialProperty( float value );
	MaterialProperty& operator=( float value );
	explicit MaterialProperty( const CXMFLOAT4& value );
	MaterialProperty& operator=( const CXMFLOAT4& value );
	explicit MaterialProperty( const char* value );
	MaterialProperty& operator=( const char* value );

private:
	enum class MaterialPropertyType
	{
		TYPE_UNKNOWN = -1,
		TYPE_INTEGER,
		TYPE_FLOAT,
		TYPE_VECTOR,
		TYPE_STRING,
	};

	MaterialPropertyStorage m_storage;
	MaterialPropertyType m_type;
};

class Material
{
public:
	void AddProperty( const char* key, int value );
	void AddProperty( const char* key, float value );
	void AddProperty( const char* key, const CXMFLOAT4& value );
	void AddProperty( const char* key, const char* value );

	int AsInteger( const char* key ) const;
	float AsFloat( const char* key ) const;
	const CXMFLOAT4& AsVector( const char* key ) const;
	const char* AsString( const char* key ) const;

	const MaterialProperty* HasProperty( const char* key ) const;

	const std::string& Name( ) const { return m_name; };

	Material( const char* name ) : m_name( name ) {}
	~Material( ) = default;
	Material( const Material& ) = default;
	Material& operator=( const Material& ) = default;
	Material( Material&& ) = default;
	Material& operator=( Material&& ) = default;

private:
	std::string m_name;
	std::map<std::string, MaterialProperty> m_properties;
};