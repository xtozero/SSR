#include "stdafx.h"
#include "Material/Material.h"

#include "Archive.h"
#include "ArchiveUtility.h"

#include <cassert>

MaterialPropertyStorage::MaterialPropertyStorage( MaterialPropertyStorage&& other )
{
	*this = std::move( other );
}

MaterialPropertyStorage& MaterialPropertyStorage::operator=( MaterialPropertyStorage&& other )
{
	if ( &other != this )
	{
		m_memory = other.m_memory;
		m_size = other.m_size;

		other.m_memory = nullptr;
		other.m_size = 0;
	}
	
	return *this;
}

void MaterialProperty::Clone( MaterialPropertyStorage& storage ) const
{
	new ( storage )MaterialProperty( *this );
}

int MaterialProperty::AsInteger( ) const
{
	assert( m_type == MaterialPropertyType::TYPE_INTEGER );
	return *static_cast<int*>( m_storage.GetRaw( ) );
}

float MaterialProperty::AsFloat( ) const
{
	assert( m_type == MaterialPropertyType::TYPE_FLOAT );
	return *static_cast<float*>( m_storage.GetRaw( ) );
}

const CXMFLOAT4& MaterialProperty::AsVector( ) const
{
	assert( m_type == MaterialPropertyType::TYPE_VECTOR );
	return *static_cast<CXMFLOAT4*>( m_storage.GetRaw( ) );
}

const char* MaterialProperty::AsString( ) const
{
	assert( m_type == MaterialPropertyType::TYPE_STRING );
	return static_cast<const char*>( m_storage.GetRaw( ) );
}

void MaterialProperty::Serialize( Archive& ar )
{
	std::size_t storageSize = m_storage.Size( );
	ar << storageSize;
	ar << m_type;
	
	if ( ar.IsWriteMode( ) == false )
	{
		m_storage.Allocate( storageSize );
	}

	switch ( m_type )
	{
	case MaterialPropertyType::TYPE_INTEGER:
		ar << *static_cast<int*>( m_storage.GetRaw( ) );
		break;
	case MaterialPropertyType::TYPE_FLOAT:
		ar << *static_cast<float*>( m_storage.GetRaw( ) );
		break;
	case MaterialPropertyType::TYPE_VECTOR:
		ar << *static_cast<CXMFLOAT4*>( m_storage.GetRaw( ) );
		break;
	case MaterialPropertyType::TYPE_STRING:
		ar << static_cast<char*>( m_storage.GetRaw( ) );
		break;
	default:
		break;
	}
}

MaterialProperty::MaterialProperty( int value )
{
	*this = value;
}

MaterialProperty & MaterialProperty::operator=( int value )
{
	m_type = MaterialPropertyType::TYPE_INTEGER;
	m_storage.Allocate( sizeof( value ) );

	*static_cast<int*>( m_storage.GetRaw( ) ) = value;

	return *this;
}

MaterialProperty::MaterialProperty( float value )
{
	*this = value;
}

MaterialProperty& MaterialProperty::operator=( float value )
{
	m_type = MaterialPropertyType::TYPE_FLOAT;
	m_storage.Allocate( sizeof( value ) );

	*static_cast<float*>( m_storage.GetRaw( ) ) = value;

	return *this;
}

MaterialProperty::MaterialProperty( const CXMFLOAT4& value )
{
	*this = value;
}

MaterialProperty& MaterialProperty::operator=( const CXMFLOAT4& value )
{
	m_type = MaterialPropertyType::TYPE_VECTOR;
	m_storage.Allocate( sizeof( value ) );

	auto& vector = *static_cast<CXMFLOAT4*>( m_storage.GetRaw( ) );
	new ( &vector )CXMFLOAT4( );

	vector = value;
	return *this;
}

MaterialProperty::MaterialProperty( const char* value )
{
	*this = value;
}

MaterialProperty& MaterialProperty::operator=( const char* value )
{
	m_type = MaterialPropertyType::TYPE_STRING;
	m_storage.Allocate( std::strlen( value ) + 1 );

	std::strcpy( static_cast<char*>( m_storage.GetRaw( ) ), value );

	return *this;
}

void MaterialProperty::Destroy( )
{
	switch ( m_type )
	{
	case MaterialProperty::MaterialPropertyType::TYPE_UNKNOWN:
		[[fallthrough]];
	case MaterialProperty::MaterialPropertyType::TYPE_INTEGER:
		// [[fallthrough]];
	case MaterialProperty::MaterialPropertyType::TYPE_FLOAT:
		break;
	case MaterialProperty::MaterialPropertyType::TYPE_VECTOR:
		{
			auto& vector = *static_cast<CXMFLOAT4*>( m_storage.GetRaw( ) );
			vector.~CXMFLOAT4( );
		}
		break;
	case MaterialProperty::MaterialPropertyType::TYPE_STRING:
		break;
	default:
		break;
	}

	m_storage.Deallocate( );
}

MaterialProperty::~MaterialProperty( )
{
	Destroy( );
}

MaterialProperty::MaterialProperty( const MaterialProperty& other )
{
	( *this ) = other;
}

MaterialProperty& MaterialProperty::operator=( const MaterialProperty& other )
{
	if ( &other != this )
	{
		if ( other.m_storage.GetRaw( ) )
		{
			other.Clone( m_storage );
		}
		else
		{
			Destroy( );
		}

		m_type = other.m_type;
	}

	return *this;
}

MaterialProperty::MaterialProperty( MaterialProperty&& other )
{
	( *this ) = std::move( other );
}

MaterialProperty& MaterialProperty::operator=( MaterialProperty&& other )
{
	if ( &other != this )
	{
		m_storage = std::move( other.m_storage );
		m_type = other.m_type;
	}
	return *this;
}

Archive& operator<<( Archive& ar, MaterialProperty& property )
{
	property.Serialize( ar );
	return ar;
}

REGISTER_ASSET( Material );
void Material::Serialize( Archive& ar )
{
	if ( ar.IsWriteMode( ) )
	{
		ar << ID;
	}

	if ( ar.IsWriteMode( ) )
	{
		ar << m_properties.size( );
		for ( auto& p : m_properties )
		{
			const std::string& propertyName = p.first;
			MaterialProperty& property = p.second;

			ar << propertyName;
			ar << property;
		}
	}
	else
	{
		std::size_t size = 0;
		ar << size;
		for ( std::size_t i = 0; i < size; ++i )
		{
			std::string propertyName;
			MaterialProperty property;

			ar << propertyName;
			ar << property;

			m_properties.emplace( std::move( propertyName ), std::move( property ) );
		}
	}
}

void Material::AddProperty( const char* key, int value )
{
	auto found = m_properties.find( key );
	if ( found == m_properties.end( ) )
	{
		m_properties.emplace( key, MaterialProperty( value ) );
	}
	else
	{
		found->second = value;
	}
}

void Material::AddProperty( const char* key, float value )
{
	auto found = m_properties.find( key );
	if ( found == m_properties.end( ) )
	{
		m_properties.emplace( key, MaterialProperty( value ) );
	}
	else
	{
		found->second = value;
	}
}

void Material::AddProperty( const char* key, const CXMFLOAT4& value )
{
	auto found = m_properties.find( key );
	if ( found == m_properties.end( ) )
	{
		m_properties.emplace( key, MaterialProperty( value ) );
	}
	else
	{
		found->second = value;
	}
}

void Material::AddProperty( const char* key, const char* value )
{
	auto found = m_properties.find( key );
	if ( found == m_properties.end( ) )
	{
		m_properties.emplace( key, MaterialProperty( value ) );
	}
	else
	{
		found->second = value;
	}
}

int Material::AsInteger( const char* key ) const
{
	auto found = m_properties.find( key );
	assert( found != m_properties.end( ) );
	return found->second.AsInteger( );
}

float Material::AsFloat( const char* key ) const
{
	auto found = m_properties.find( key );
	assert( found != m_properties.end( ) );
	return found->second.AsFloat( );
}

const CXMFLOAT4& Material::AsVector( const char* key ) const
{
	auto found = m_properties.find( key );
	assert( found != m_properties.end( ) );
	return found->second.AsVector( );
}

const char* Material::AsString( const char* key ) const
{
	auto found = m_properties.find( key );
	assert( found != m_properties.end( ) );
	return found->second.AsString( );
}

const MaterialProperty* Material::HasProperty( const char* key ) const
{
	auto found = m_properties.find( key );
	if ( found != m_properties.end( ) )
	{
		return &found->second;
	}

	return nullptr;
}
