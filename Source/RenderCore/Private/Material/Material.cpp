#include "stdafx.h"
#include "Material/Material.h"

#include "Archive.h"
#include "ArchiveUtility.h"
#include "Material/MaterialResource.h"

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

void FloatProperty::CopyValue( void* dest ) const
{
	std::memcpy( dest, &m_value, sizeof( m_value ) );
}

void IntProperty::Serialize( Archive& ar )
{
	ar << m_value;
}

void IntProperty::CopyValue( void* dest ) const
{
	std::memcpy( dest, &m_value, sizeof( m_value ) );
}

void Float4Property::Serialize( Archive& ar )
{
	ar << m_value;
}

void Float4Property::CopyValue( void * dest ) const
{
	std::memcpy( dest, &m_value, sizeof( m_value ) );
}

void TextureProperty::Serialize( Archive& ar )
{
	ar << m_value;
}

void TextureProperty::CopyValue( void* dest ) const
{
	Texture* raw = m_value.get( );
	std::memcpy( dest, &raw, sizeof( raw ) );
}

REGISTER_ASSET( Material );
void Material::Serialize( Archive& ar )
{
	if ( ar.IsWriteMode( ) )
	{
		ar << ID;
	}

	ar << m_name;

	ar << m_vertexShader;
	ar << m_pixelShader;

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

		ar << m_samplers.size( );
		for ( auto& s : m_samplers )
		{
			ar << s.first;
			ar << s.second;
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

		ar << size;
		for ( std::size_t i = 0; i < size; ++i )
		{
			std::string samplerName;
			ar << samplerName;
			ar << m_samplers[samplerName];
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

const MaterialProperty* Material::AsProperty( const char* key ) const
{
	auto found = m_properties.find( key );
	if ( found != m_properties.end( ) )
	{
		return found->second.get( );
	}

	return nullptr;
}

int Material::AsInteger( const char* key ) const
{
	auto found = m_properties.find( key );

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

	if ( found != m_properties.end( ) )
	{
		const std::unique_ptr<MaterialProperty>& property = found->second;
		if ( property && ( property->Type( ) == MaterialPropertyType::Float4 ) )
		{
			auto concreteProperty = static_cast<Float4Property*>( found->second.get( ) );
			return concreteProperty->Value( );
		}
	}

	static CXMFLOAT4 zeroVector( 0.f, 0.f, 0.f, 0.f );
	return zeroVector;
}

Texture* Material::AsTexture( const char* key ) const
{
	auto found = m_properties.find( key );

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

SamplerOption* Material::AsSampelrOption( const char* key ) const
{
	auto found = m_samplers.find( key );

	if ( found != m_samplers.end( ) )
	{
		return found->second.get( );
	}

	return nullptr;
}

void Material::CopyProperty( const char* key, void* dest ) const
{
	auto found = m_properties.find( key );

	if ( found != m_properties.end( ) )
	{
		MaterialProperty& property = *found->second;
		property.CopyValue( dest );
	}
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

void Material::SetVertexShader( const std::shared_ptr<VertexShader>& vertexshader )
{
	m_vertexShader = vertexshader;
}

const ShaderBase* Material::GetShader( SHADER_TYPE type ) const
{
	switch ( type )
	{
	case SHADER_TYPE::NONE:
		break;
	case SHADER_TYPE::VS:
		return m_vertexShader.get( );
		break;
	case SHADER_TYPE::HS:
		break;
	case SHADER_TYPE::DS:
		break;
	case SHADER_TYPE::GS:
		break;
	case SHADER_TYPE::PS:
		return m_pixelShader.get( );
		break;
	case SHADER_TYPE::CS:
		break;
	case SHADER_TYPE::Count:
		[[fallthrough]];
	default:
		break;
	}

	return nullptr;
}

const VertexShader* Material::GetVertexShader( ) const
{
	return m_vertexShader.get();
}

const PixelShader* Material::GetPixelShader( ) const
{
	return m_pixelShader.get();
}

void Material::SetPixelShader( const std::shared_ptr<PixelShader>& pixelShader )
{
	m_pixelShader = pixelShader;
}

void Material::AddSampler( const std::string& key, const std::shared_ptr<SamplerOption>& samplerOption )
{
	m_samplers.emplace( key, samplerOption );
}

MaterialResource* Material::GetMaterialResource( ) const
{
	return m_materialResource.get( );
}

Material::Material( const char* name ) : m_name( name )
{
}

Material::Material( ) = default;

Material::~Material( )
{
}

Material::Material( Material&& other )
{
	*this = std::move( other );
}

Material& Material::operator=( Material&& other )
{
	if ( this != &other )
	{
		m_path = std::move( other.m_path );
		m_name = std::move( other.m_name );
		m_properties = std::move( other.m_properties );
		m_materialResource = std::move( other.m_materialResource );
	}

	return *this;
}

void Material::PostLoadImpl( )
{
	m_materialResource = std::make_unique<MaterialResource>( );
	m_materialResource->SetMaterial( std::static_pointer_cast<Material>( SharedThis( ) ) );
}
