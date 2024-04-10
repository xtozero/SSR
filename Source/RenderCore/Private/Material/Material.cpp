#include "Material/Material.h"

#include "Archive.h"
#include "ArchiveUtility.h"
#include "Material/MaterialResource.h"
#include "UberShader.h"

#include <cassert>

using ::rendercore::FloatProperty;
using ::rendercore::Float4Property;
using ::rendercore::IntProperty;
using ::rendercore::MaterialProperty;
using ::rendercore::MaterialPropertyType;
using ::rendercore::TextureProperty;

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

namespace rendercore
{
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

	void Float4Property::CopyValue( void* dest ) const
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

				m_properties.emplace( Name( propertyName ), std::move( property ) );
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

	const std::map<Name, std::unique_ptr<MaterialProperty>>& Material::GetProperties() const
	{
		return m_properties;
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

	const SamplerOption* Material::AsSampelrOption( const char* key ) const
	{
		auto found = m_samplers.find( Name( key ) );

		if ( found != m_samplers.end() )
		{
			return &found->second;
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

	void Material::SetName( const Name& name )
	{
		m_name = name;
	}

	const ShaderBase* Material::GetShader( agl::ShaderType type ) const
	{
		switch ( type )
		{
		case agl::ShaderType::None:
			break;
		case agl::ShaderType::VS:
			return GetVertexShader();
			break;
		case agl::ShaderType::HS:
			break;
		case agl::ShaderType::DS:
			break;
		case agl::ShaderType::GS:
			return GetGeometryShader();
			break;
		case agl::ShaderType::PS:
			return GetPixelShader();
			break;
		case agl::ShaderType::CS:
			break;
		case agl::ShaderType::Count:
			[[fallthrough]];
		default:
			break;
		}

		return nullptr;
	}

	void Material::SetVertexShader( const std::shared_ptr<VertexShader>& vertexshader )
	{
		m_shaders[static_cast<uint32>( agl::ShaderType::VS )] = vertexshader;
	}

	const VertexShader* Material::GetVertexShader( const StaticShaderSwitches* switches ) const
	{
		auto& vertexShader = m_shaders[static_cast<uint32>( agl::ShaderType::VS )];
		if ( vertexShader == nullptr )
		{
			return nullptr;
		}

		const StaticShaderSwitches& compileOption = switches ? *switches : m_shaderSwitches[static_cast<uint32>( agl::ShaderType::VS )];

		const ShaderBase* compiled = vertexShader->CompileShader( compileOption );
		return static_cast<const VertexShader*>( compiled );
	}

	VertexShader* Material::GetVertexShader( const StaticShaderSwitches* switches )
	{
		auto& vertexShader = m_shaders[static_cast<uint32>( agl::ShaderType::VS )];
		if ( vertexShader == nullptr )
		{
			return nullptr;
		}

		const StaticShaderSwitches& compileOption = switches ? *switches : m_shaderSwitches[static_cast<uint32>( agl::ShaderType::VS )];

		ShaderBase* compiled = vertexShader->CompileShader( compileOption );
		return static_cast<VertexShader*>( compiled );
	}

	void Material::SetGeometryShader( const std::shared_ptr<GeometryShader>& geometryShader )
	{
		m_shaders[static_cast<uint32>( agl::ShaderType::GS )] = geometryShader;
	}

	const GeometryShader* Material::GetGeometryShader( const StaticShaderSwitches* switches ) const
	{
		auto& geometryShader = m_shaders[static_cast<uint32>( agl::ShaderType::GS )];
		if ( geometryShader == nullptr )
		{
			return nullptr;
		}

		const StaticShaderSwitches& compileOption = switches ? *switches : m_shaderSwitches[static_cast<uint32>( agl::ShaderType::GS )];

		const ShaderBase* compiled = geometryShader->CompileShader( compileOption );
		return static_cast<const GeometryShader*>( compiled );
	}

	GeometryShader* Material::GetGeometryShader( const StaticShaderSwitches* switches )
	{
		auto& geometryShader = m_shaders[static_cast<uint32>( agl::ShaderType::GS )];
		if ( geometryShader == nullptr )
		{
			return nullptr;
		}

		const StaticShaderSwitches& compileOption = switches ? *switches : m_shaderSwitches[static_cast<uint32>( agl::ShaderType::GS )];

		ShaderBase* compiled = geometryShader->CompileShader( compileOption );
		return static_cast<GeometryShader*>( compiled );
	}

	void Material::SetPixelShader( const std::shared_ptr<PixelShader>& pixelShader )
	{
		m_shaders[static_cast<uint32>( agl::ShaderType::PS )] = pixelShader;
	}

	const PixelShader* Material::GetPixelShader( const StaticShaderSwitches* switches ) const
	{
		auto& pixelShader = m_shaders[static_cast<uint32>( agl::ShaderType::PS )];
		if ( pixelShader == nullptr )
		{
			return nullptr;
		}

		const StaticShaderSwitches& compileOption = switches ? *switches : m_shaderSwitches[static_cast<uint32>( agl::ShaderType::PS )];

		const ShaderBase* compiled = pixelShader->CompileShader( compileOption );
		return static_cast<const PixelShader*>( compiled );
	}

	PixelShader* Material::GetPixelShader( const StaticShaderSwitches* switches )
	{
		auto& pixelShader = m_shaders[static_cast<uint32>( agl::ShaderType::PS )];
		if ( pixelShader == nullptr )
		{
			return nullptr;
		}

		const StaticShaderSwitches& compileOption = switches ? *switches : m_shaderSwitches[static_cast<uint32>( agl::ShaderType::PS )];

		ShaderBase* compiled = pixelShader->CompileShader( compileOption );
		return static_cast<PixelShader*>( compiled );
	}

	void Material::AddSampler( const std::string& key, const SamplerOption& samplerOption )
	{
		m_samplers.emplace( Name( key ), samplerOption );
	}

	StaticShaderSwitches Material::GetShaderSwitches( agl::ShaderType type )
	{
		auto& shader = m_shaders[static_cast<uint32>( type )];
		if ( shader != nullptr )
		{
			return shader->GetStaticSwitches();
		}

		return StaticShaderSwitches();
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
		for ( uint32 i = 0; i < agl::MAX_SHADER_TYPE<uint32>; ++i )
		{
			if ( auto shader = m_shaders[i].get() )
			{
				m_shaderSwitches[i] = shader->GetStaticSwitches();
			}

			for ( const auto& define : m_defines[i] )
			{
				m_shaderSwitches[i].On( define.first, define.second );
			}
		}

		m_materialResource = std::make_unique<MaterialResource>();
		m_materialResource->SetMaterial( std::static_pointer_cast<Material>( SharedThis() ) );
	}
}
