#include "stdafx.h"
#include "Material/Material.h"

#include "Archive.h"
#include "ArchiveUtility.h"
#include "Material/MaterialResource.h"
#include "UberShader.h"

#include <cassert>

namespace
{
	MaterialProperty* CreateMaterialPropertyByType( MaterialPropertyType type )
	{
		switch ( type )
		{
		case MaterialPropertyType::Float:
			return new FloatProperty();
			break;
		case MaterialPropertyType::Float4:
			return new Float4Property();
			break;
		case MaterialPropertyType::Int:
			return new IntProperty();
			break;
		case MaterialPropertyType::Texture:
			return new TextureProperty();
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
	Texture* raw = m_value.get();
	std::memcpy( dest, &raw, sizeof( raw ) );
}

REGISTER_ASSET( Material );
void Material::Serialize( Archive& ar )
{
	Super::Serialize( ar );

	if ( ar.IsWriteMode() )
	{
		ar << static_cast<uint32>( m_properties.size() );
		for ( auto& p : m_properties )
		{
			const std::string_view& propertyName = p.first.Str();
			const std::unique_ptr<MaterialProperty>& property = p.second;

			ar << propertyName;
			ar << property->Type();
			property->Serialize( ar );
		}
	}
	else
	{
		uint32 size = 0;
		ar << size;
		for ( uint32 i = 0; i < size; ++i )
		{
			char propertyName[NameSize];
			MaterialPropertyType type;
			MaterialProperty* property;

			ar << propertyName;
			ar << type;
			property = CreateMaterialPropertyByType( type );
			property->Serialize( ar );

			m_properties.emplace( Name( propertyName ), std::move(property));
		}
	}
}

void Material::AddProperty( const char* key, int32 value )
{
	auto found = m_properties.find( Name( key ) );
	if ( found != m_properties.end() )
	{
		const std::unique_ptr<MaterialProperty>& property = found->second;
		if ( property && ( property->Type() != MaterialPropertyType::Int ) )
		{
			m_properties.erase( found );
			found = m_properties.end();
		}
	}

	if ( found == m_properties.end() )
	{
		auto result = m_properties.emplace( key, new IntProperty( value ) );
		found = result.first;
		return;
	}

	std::construct_at( static_cast<IntProperty*>( found->second.get() ), value );
}

void Material::AddProperty( const char* key, float value )
{
	auto found = m_properties.find( Name( key ) );
	if ( found != m_properties.end() )
	{
		const std::unique_ptr<MaterialProperty>& property = found->second;
		if ( property && ( property->Type() != MaterialPropertyType::Int ) )
		{
			m_properties.erase( found );
			found = m_properties.end();
		}
	}

	if ( found == m_properties.end() )
	{
		auto result = m_properties.emplace( key, new FloatProperty( value ) );
		found = result.first;
		return;
	}

	std::construct_at( static_cast<FloatProperty*>( found->second.get() ), value );
}

void Material::AddProperty( const char* key, const Vector4& value )
{
	auto found = m_properties.find( Name( key ) );
	if ( found != m_properties.end() )
	{
		const std::unique_ptr<MaterialProperty>& property = found->second;
		if ( property && ( property->Type() != MaterialPropertyType::Int ) )
		{
			m_properties.erase( found );
			found = m_properties.end();
		}
	}

	if ( found == m_properties.end() )
	{
		auto result = m_properties.emplace( key, new Float4Property( value ) );
		found = result.first;
		return;
	}

	std::construct_at( static_cast<Float4Property*>( found->second.get() ), value );
}

void Material::AddProperty( const char* key, const std::shared_ptr<Texture>& value )
{
	auto found = m_properties.find( Name( key ) );
	if ( found != m_properties.end() )
	{
		const std::unique_ptr<MaterialProperty>& property = found->second;
		if ( property && ( property->Type() != MaterialPropertyType::Int ) )
		{
			m_properties.erase( found );
			found = m_properties.end();
		}
	}

	if ( found == m_properties.end() )
	{
		auto result = m_properties.emplace( key, new TextureProperty( value ) );
		found = result.first;
		return;
	}

	std::construct_at( static_cast<TextureProperty*>( found->second.get() ), value );
}

const MaterialProperty* Material::AsProperty( const char* key ) const
{
	auto found = m_properties.find( Name( key ) );
	if ( found != m_properties.end() )
	{
		return found->second.get();
	}

	return nullptr;
}

int32 Material::AsInteger( const char* key ) const
{
	auto found = m_properties.find( Name( key ) );

	if ( found != m_properties.end() )
	{
		const auto& property = found->second;
		if ( property && ( property->Type() == MaterialPropertyType::Int ) )
		{
			auto concreteProperty = static_cast<IntProperty*>( found->second.get() );
			return concreteProperty->Value();
		}
	}

	return 0;
}

float Material::AsFloat( const char* key ) const
{
	auto found = m_properties.find( Name( key ) );

	if ( found != m_properties.end() )
	{
		const std::unique_ptr<MaterialProperty>& property = found->second;
		if ( property && ( property->Type() == MaterialPropertyType::Float ) )
		{
			auto concreteProperty = static_cast<FloatProperty*>( found->second.get() );
			return concreteProperty->Value();
		}
	}

	return 0.f;
}

const Vector4& Material::AsVector( const char* key ) const
{
	auto found = m_properties.find( Name( key ) );

	if ( found != m_properties.end() )
	{
		const std::unique_ptr<MaterialProperty>& property = found->second;
		if ( property && ( property->Type() == MaterialPropertyType::Float4 ) )
		{
			auto concreteProperty = static_cast<Float4Property*>( found->second.get() );
			return concreteProperty->Value();
		}
	}

	return Vector4::ZeroVector;
}

Texture* Material::AsTexture( const char* key ) const
{
	auto found = m_properties.find( Name( key ) );

	if ( found != m_properties.end() )
	{
		const std::unique_ptr<MaterialProperty>& property = found->second;
		if ( property && ( property->Type() == MaterialPropertyType::Texture ) )
		{
			auto concreteProperty = static_cast<TextureProperty*>( found->second.get() );
			return concreteProperty->Value().get();
		}
	}

	return nullptr;
}

SamplerOption* Material::AsSampelrOption( const char* key ) const
{
	auto found = m_samplers.find( Name( key ) );

	if ( found != m_samplers.end() )
	{
		return found->second.get();
	}

	return nullptr;
}

void Material::CopyProperty( const char* key, void* dest ) const
{
	auto found = m_properties.find( Name( key ) );

	if ( found != m_properties.end() )
	{
		MaterialProperty& property = *found->second;
		property.CopyValue( dest );
	}
}

bool Material::HasProperty( const char* key ) const
{
	auto found = m_properties.find( Name( key ) );
	if ( found != m_properties.end() )
	{
		return found->second != nullptr;
	}

	return false;
}

const ShaderBase* Material::GetShader( SHADER_TYPE type ) const
{
	switch ( type )
	{
	case SHADER_TYPE::NONE:
		break;
	case SHADER_TYPE::VS:
		return GetVertexShader();
		break;
	case SHADER_TYPE::HS:
		break;
	case SHADER_TYPE::DS:
		break;
	case SHADER_TYPE::GS:
		return GetGeometryShader();
		break;
	case SHADER_TYPE::PS:
		return GetPixelShader();
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

void Material::SetVertexShader( const std::shared_ptr<VertexShader>& vertexshader )
{
	m_shaders[static_cast<uint32>( SHADER_TYPE::VS )] = vertexshader;
}

const VertexShader* Material::GetVertexShader() const
{
	auto& vertexShader = m_shaders[static_cast<uint32>( SHADER_TYPE::VS )];
	if ( vertexShader == nullptr )
	{
		return nullptr;
	}

	const ShaderBase* compiled = vertexShader->CompileShader( m_shaderSwitches[static_cast<uint32>( SHADER_TYPE::VS )] );
	return static_cast<const VertexShader*>( compiled );
}

VertexShader* Material::GetVertexShader()
{
	auto& vertexShader = m_shaders[static_cast<uint32>( SHADER_TYPE::VS )];
	if ( vertexShader == nullptr )
	{
		return nullptr;
	}

	ShaderBase* compiled = vertexShader->CompileShader( m_shaderSwitches[static_cast<uint32>( SHADER_TYPE::VS )] );
	return static_cast<VertexShader*>( compiled );
}

void Material::SetGeometryShader( const std::shared_ptr<GeometryShader>& geometryShader )
{
	m_shaders[static_cast<uint32>( SHADER_TYPE::GS )] = geometryShader;
}

const GeometryShader* Material::GetGeometryShader() const
{
	auto& geometryShader = m_shaders[static_cast<uint32>( SHADER_TYPE::GS )];
	if ( geometryShader == nullptr )
	{
		return nullptr;
	}

	const ShaderBase* compiled = geometryShader->CompileShader( m_shaderSwitches[static_cast<uint32>( SHADER_TYPE::GS )] );
	return static_cast<const GeometryShader*>( compiled );
}

GeometryShader* Material::GetGeometryShader()
{
	auto& geometryShader = m_shaders[static_cast<uint32>( SHADER_TYPE::GS )];
	if ( geometryShader == nullptr )
	{
		return nullptr;
	}

	ShaderBase* compiled = geometryShader->CompileShader( m_shaderSwitches[static_cast<uint32>( SHADER_TYPE::GS )] );
	return static_cast<GeometryShader*>( compiled );
}

void Material::SetPixelShader( const std::shared_ptr<PixelShader>& pixelShader )
{
	m_shaders[static_cast<uint32>( SHADER_TYPE::PS )] = pixelShader;
}

const PixelShader* Material::GetPixelShader() const
{
	auto& pixelShader = m_shaders[static_cast<uint32>( SHADER_TYPE::PS )];
	if ( pixelShader == nullptr )
	{
		return nullptr;
	}

	const ShaderBase* compiled = pixelShader->CompileShader( m_shaderSwitches[static_cast<uint32>( SHADER_TYPE::PS )] );
	return static_cast<const PixelShader*>( compiled );
}

PixelShader* Material::GetPixelShader()
{
	auto& pixelShader = m_shaders[static_cast<uint32>( SHADER_TYPE::PS )];
	if ( pixelShader == nullptr )
	{
		return nullptr;
	}

	ShaderBase* compiled = pixelShader->CompileShader( m_shaderSwitches[static_cast<uint32>( SHADER_TYPE::PS )] );
	return static_cast<PixelShader*>( compiled );
}

void Material::AddSampler( const std::string& key, const std::shared_ptr<SamplerOption>& samplerOption )
{
	m_samplers.emplace( Name( key ), samplerOption );
}

MaterialResource* Material::GetMaterialResource() const
{
	return m_materialResource.get();
}

Material::Material( const char* name ) : m_name( name )
{
}

Material::Material() = default;

Material::~Material() = default;

Material::Material( Material&& other ) = default;

Material& Material::operator=( Material&& other ) = default;

void Material::PostLoadImpl()
{
	for ( int i = 0; i < MAX_SHADER_TYPE<uint32>; ++i )
	{
		if ( auto uberShader = Cast<rendercore::UberShader>( m_shaders[i].get() ) )
		{
			m_shaderSwitches[i] = uberShader->Switches();
		}

		for ( const auto& define : m_defines[i] )
		{
			m_shaderSwitches[i].On( define.first, define.second );
		}
	}

	m_materialResource = std::make_unique<MaterialResource>();
	m_materialResource->SetMaterial( std::static_pointer_cast<Material>( SharedThis() ) );
}
