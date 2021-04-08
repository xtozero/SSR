#include "stdafx.h"
#include "Material/Material.h"

#include "Archive.h"
#include "ArchiveUtility.h"

#include <cassert>

namespace
{
	MaterialProperty* CreateMaterialPropertyByType( MaterialPropertyType type )
	{
		switch ( type )
		{
		case MaterialPropertyType::Float:
			return new FloatProperty( );
			break;
		case MaterialPropertyType::Float4:
			return new Float4Property( );
			break;
		case MaterialPropertyType::Int:
			return new IntProperty( );
			break;
		case MaterialPropertyType::Texture:
			return new TextureProperty( );
			break;
		default:
			break;
		}

		assert( false );
		return nullptr;
	}
}

void FloatProperty::Serialize( Archive& ar )
{
	ar << m_value;
}

void IntProperty::Serialize( Archive& ar )
{
	ar << m_value;
}

void Float4Property::Serialize( Archive& ar )
{
	ar << m_value;
}

void TextureProperty::Serialize( Archive& ar )
{
	ar << m_value;
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
			const std::unique_ptr<MaterialProperty>& property = p.second;

			ar << propertyName;
			ar << property->Type( );
			property->Serialize( ar );
		}
	}
	else
	{
		std::size_t size = 0;
		ar << size;
		for ( std::size_t i = 0; i < size; ++i )
		{
			std::string propertyName;
			MaterialPropertyType type;
			MaterialProperty* property;

			ar << propertyName;
			ar << type;
			property = CreateMaterialPropertyByType( type );
			property->Serialize( ar );

			m_properties.emplace( std::move( propertyName ), std::move( property ) );
		}
	}
}

void Material::AddProperty( const char* key, int value )
{
	auto found = m_properties.find( key );
	if ( found != m_properties.end( ) )
	{
		const std::unique_ptr<MaterialProperty>& property = found->second;
		if ( property && ( property->Type( ) != MaterialPropertyType::Int ) )
		{
			m_properties.erase( found );
			found = m_properties.end( );
		}
	}

	if ( found == m_properties.end( ) )
	{
		auto result = m_properties.emplace( key, new IntProperty( value ) );
		found = result.first;
		return;
	}

	new ( found->second.get( ) )IntProperty( value );
}

void Material::AddProperty( const char* key, float value )
{
	auto found = m_properties.find( key );
	if ( found != m_properties.end( ) )
	{
		const std::unique_ptr<MaterialProperty>& property = found->second;
		if ( property && ( property->Type( ) != MaterialPropertyType::Int ) )
		{
			m_properties.erase( found );
			found = m_properties.end( );
		}
	}

	if ( found == m_properties.end( ) )
	{
		auto result = m_properties.emplace( key, new FloatProperty( value ) );
		found = result.first;
		return;
	}

	new ( found->second.get( ) )FloatProperty( value );
}

void Material::AddProperty( const char* key, const CXMFLOAT4& value )
{
	auto found = m_properties.find( key );
	if ( found != m_properties.end( ) )
	{
		const std::unique_ptr<MaterialProperty>& property = found->second;
		if ( property && ( property->Type( ) != MaterialPropertyType::Int ) )
		{
			m_properties.erase( found );
			found = m_properties.end( );
		}
	}

	if ( found == m_properties.end( ) )
	{
		auto result = m_properties.emplace( key, new Float4Property( value ) );
		found = result.first;
		return;
	}

	new ( found->second.get( ) )Float4Property( value );
}

void Material::AddProperty( const char* key, const std::shared_ptr<Texture>& value )
{
	auto found = m_properties.find( key );
	if ( found != m_properties.end( ) )
	{
		const std::unique_ptr<MaterialProperty>& property = found->second;
		if ( property && ( property->Type( ) != MaterialPropertyType::Int ) )
		{
			m_properties.erase( found );
			found = m_properties.end( );
		}
	}

	if ( found == m_properties.end( ) )
	{
		auto result = m_properties.emplace( key, new TextureProperty( value ) );
		found = result.first;
		return;
	}

	new ( found->second.get( ) )TextureProperty( value );
}

int Material::AsInteger( const char* key ) const
{
	auto found = m_properties.find( key );
	assert( found != m_properties.end( ) );

	if ( found != m_properties.end( ) )
	{
		const auto& property = found->second;
		if ( property && ( property->Type( ) == MaterialPropertyType::Int ) )
		{
			auto concreteProperty = static_cast<IntProperty*>( found->second.get( ) );
			return concreteProperty->Value( );
		}
	}

	return 0;
}

float Material::AsFloat( const char* key ) const
{
	auto found = m_properties.find( key );
	assert( found != m_properties.end( ) );

	if ( found != m_properties.end( ) )
	{
		const std::unique_ptr<MaterialProperty>& property = found->second;
		if ( property && ( property->Type( ) == MaterialPropertyType::Float ) )
		{
			auto concreteProperty = static_cast<FloatProperty*>( found->second.get( ) );
			return concreteProperty->Value( );
		}
	}

	return 0.f;
}

const CXMFLOAT4& Material::AsVector( const char* key ) const
{
	auto found = m_properties.find( key );
	assert( found != m_properties.end( ) );

	if ( found != m_properties.end( ) )
	{
		const std::unique_ptr<MaterialProperty>& property = found->second;
		if ( property && ( property->Type( ) == MaterialPropertyType::Float4 ) )
		{
			auto concreteProperty = static_cast<Float4Property*>( found->second.get( ) );
			return concreteProperty->Value( );
		}
	}

	return CXMFLOAT4( 0.f, 0.f, 0.f, 0.f );
}

Texture* Material::AsTexture( const char* key ) const
{
	auto found = m_properties.find( key );
	assert( found != m_properties.end( ) );

	if ( found != m_properties.end( ) )
	{
		const std::unique_ptr<MaterialProperty>& property = found->second;
		if ( property && ( property->Type( ) == MaterialPropertyType::Texture ) )
		{
			auto concreteProperty = static_cast<TextureProperty*>( found->second.get( ) );
			return concreteProperty->Value( ).get( );
		}
	}

	return nullptr;
}

bool Material::HasProperty( const char* key ) const
{
	auto found = m_properties.find( key );
	if ( found != m_properties.end( ) )
	{
		return found->second != nullptr;
	}

	return nullptr;
}

void Material::PostLoadImpl( )
{
}
