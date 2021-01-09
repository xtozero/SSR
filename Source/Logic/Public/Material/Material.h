#pragma once

#include "AssetLoader/AssetFactory.h"
#include "common.h"
#include "Core/IAsyncLoadableAsset.h"
#include "Math/CXMFloat.h"

#include <filesystem>
#include <map>
#include <string>

class Archive;

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

	std::size_t Size( ) const
	{
		return m_size;
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

	void Serialize( Archive& ar );

	MaterialProperty( ) = default;
	LOGIC_DLL ~MaterialProperty( );
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
	void Destroy( );

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

class Material : public AsyncLoadableAsset
{
	DECLARE_ASSET( Material );
public:
	LOGIC_DLL virtual void Serialize( Archive& ar ) override;

	const std::filesystem::path& Path( ) const { return m_path; }
	void SetPath( const std::filesystem::path& path ) { m_path = path; }

	LOGIC_DLL void AddProperty( const char* key, int value );
	LOGIC_DLL void AddProperty( const char* key, float value );
	LOGIC_DLL void AddProperty( const char* key, const CXMFLOAT4& value );
	LOGIC_DLL void AddProperty( const char* key, const char* value );

	int AsInteger( const char* key ) const;
	float AsFloat( const char* key ) const;
	const CXMFLOAT4& AsVector( const char* key ) const;
	const char* AsString( const char* key ) const;

	const MaterialProperty* HasProperty( const char* key ) const;

	const std::string& Name( ) const { return m_name; }

	LOGIC_DLL Material( const char* name ) : m_name( name ) {}
	LOGIC_DLL Material( ) = default;
	Material( const Material& ) = default;
	Material& operator=( const Material& ) = default;
	Material( Material&& ) = default;
	Material& operator=( Material&& ) = default;

private:
	std::filesystem::path m_path;

	std::string m_name;
	std::map<std::string, MaterialProperty> m_properties;
};