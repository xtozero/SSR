#include "Material/Material.h"

#include "Archive.h"
#include "ArchiveUtility.h"
#include "Config/DefaultRenderCoreConfig.h"
#include "Material/MaterialResource.h"
#include "SparseArray.h"
#include "TextureStreaming.h"

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
		if ( found != std::end( m_properties ) )
		{
			const std::unique_ptr<MaterialProperty>& property = found->second;
			if ( property && ( property->Type() != MaterialPropertyType::Int ) )
			{
				m_properties.erase( found );
				found = std::end( m_properties );
			}
		}

		if ( found == std::end( m_properties ) )
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
		if ( found != std::end( m_properties ) )
		{
			const std::unique_ptr<MaterialProperty>& property = found->second;
			if ( property && ( property->Type() != MaterialPropertyType::Int ) )
			{
				m_properties.erase( found );
				found = std::end( m_properties );
			}
		}

		if ( found == std::end( m_properties ) )
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
		if ( found != std::end( m_properties ) )
		{
			const std::unique_ptr<MaterialProperty>& property = found->second;
			if ( property && ( property->Type() != MaterialPropertyType::Int ) )
			{
				m_properties.erase( found );
				found = std::end( m_properties );
			}
		}

		if ( found == std::end( m_properties ) )
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
		if ( found != std::end( m_properties ) )
		{
			const std::unique_ptr<MaterialProperty>& property = found->second;
			if ( property && ( property->Type() != MaterialPropertyType::Int ) )
			{
				m_properties.erase( found );
				found = std::end( m_properties );
			}
		}

		if ( found == std::end( m_properties ) )
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
		if ( found != std::end( m_properties ) )
		{
			return found->second.get();
		}

		return nullptr;
	}

	int32 Material::AsInteger( const char* key ) const
	{
		auto found = m_properties.find( Name( key ) );
		if ( found != std::end( m_properties ) )
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
		if ( found != std::end( m_properties ) )
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
		if ( found != std::end( m_properties ) )
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
		if ( found != std::end( m_properties ) )
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
		if ( found != std::end( m_samplers ) )
		{
			return &found->second;
		}

		return nullptr;
	}

	void Material::CopyProperty( const char* key, void* dest ) const
	{
		auto found = m_properties.find( Name( key ) );
		if ( found != std::end( m_properties ) )
		{
			MaterialProperty& property = *found->second;
			property.CopyValue( dest );
		}
	}

	bool Material::HasProperty( const char* key ) const
	{
		auto found = m_properties.find( Name( key ) );
		if ( found != std::end( m_properties ) )
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
		case agl::ShaderType::Vertex:
			return GetVertexShader();
			break;
		case agl::ShaderType::Hull:
			break;
		case agl::ShaderType::Domain:
			break;
		case agl::ShaderType::Geometry:
			return GetGeometryShader();
			break;
		case agl::ShaderType::Pixel:
			return GetPixelShader();
			break;
		case agl::ShaderType::Compute:
			return GetComputeShader();
			break;
		case agl::ShaderType::Mesh:
			return GetMeshShader();
			break;
		case agl::ShaderType::Amplification:
			return GetAmplificationShader();
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
		m_shaders[static_cast<uint32>( agl::ShaderType::Vertex )] = vertexshader;
	}

	VertexShader* Material::GetVertexShader( const IShaderPermutation* permutation ) const
	{
		return static_cast<VertexShader*>( GetCompiledShader( agl::ShaderType::Vertex, permutation ) );
	}

	void Material::SetGeometryShader( const std::shared_ptr<GeometryShader>& geometryShader )
	{
		m_shaders[static_cast<uint32>( agl::ShaderType::Geometry )] = geometryShader;
	}

	GeometryShader* Material::GetGeometryShader( const IShaderPermutation* permutation ) const
	{
		return static_cast<GeometryShader*>( GetCompiledShader( agl::ShaderType::Geometry, permutation ) );
	}

	void Material::SetPixelShader( const std::shared_ptr<PixelShader>& pixelShader )
	{
		m_shaders[static_cast<uint32>( agl::ShaderType::Pixel )] = pixelShader;
	}

	PixelShader* Material::GetPixelShader( const IShaderPermutation* permutation ) const
	{
		return static_cast<PixelShader*>( GetCompiledShader( agl::ShaderType::Pixel, permutation ) );
	}

	void Material::SetComputeShader( const std::shared_ptr<ComputeShader>& computeShader )
	{
		m_shaders[static_cast<uint32>( agl::ShaderType::Compute )] = computeShader;
	}

	ComputeShader* Material::GetComputeShader( const IShaderPermutation* permutation ) const
	{
		return static_cast<ComputeShader*>( GetCompiledShader( agl::ShaderType::Compute, permutation ) );
	}

	void Material::SetMeshShader( const std::shared_ptr<MeshShader>& meshShader )
	{
		m_shaders[static_cast<uint32>( agl::ShaderType::Mesh )] = meshShader;
	}

	MeshShader* Material::GetMeshShader( const IShaderPermutation* permutation ) const
	{
		return static_cast<MeshShader*>( GetCompiledShader( agl::ShaderType::Mesh, permutation ) );
	}

	void Material::SetAmplificationShader( const std::shared_ptr<AmplificationShader>& amplificationShader )
	{
		m_shaders[static_cast<uint32>( agl::ShaderType::Amplification )] = amplificationShader;
	}

	AmplificationShader* Material::GetAmplificationShader( const IShaderPermutation* permutation ) const
	{
		return static_cast<AmplificationShader*>( GetCompiledShader( agl::ShaderType::Amplification, permutation ) );
	}

	void Material::AddSampler( const std::string& key, const SamplerOption& samplerOption )
	{
		m_samplers.emplace( Name( key ), samplerOption );
	}

	bool Material::UseMeshShader() const
	{
		return GetInterface<agl::IAgl>()->SupportsMeshShader() && HasShaderSource( agl::ShaderType::Mesh );
	}

	bool Material::SupportsVisibilityRendering() const
	{
		return DefaultRenderCore::SupportsVisibilityRendering() && HasShaderSource( agl::ShaderType::Compute );
	}

	IShaderPermutation& Material::GetShaderPermutation( agl::ShaderType type )
	{
		auto index = static_cast<uint32>( type );
		if ( m_permutation[index].IsValid() )
		{
			return *m_permutation[index].Get();
		}

		static ShaderPermutation<> empty;
		return empty;
	}

	MaterialResource* Material::GetMaterialResource() const
	{
		return m_materialResource.get();
	}

	int32 Material::GetMaterialId() const
	{
		return m_materialId;
	}

	void Material::RequestMipLevels( int32 desiredMipLevel )
	{
		for ( auto& [name, property] : m_properties )
		{
			if ( auto textureProperty = Cast<const TextureProperty>( property.get() ) )
			{
				if ( Texture* texture = textureProperty->Value().get() )
				{
					texture->RequestMipLevels( desiredMipLevel );
				}
			}
		}
	}

	Material::Material( const char* name ) : m_name( name )
	{
	}

	// unique_ptr for incomplete type
	Material::Material() = default;
	Material::~Material()
	{
		if ( m_materialId >= 0 )
		{
			auto lambda = [id = m_materialId]()
			{
				TextureStreamingManager::GetInstance().Unregister( id );
			};

			if ( IsInGameThread() )
			{
				lambda();
			}
			else
			{
				EnqueueThreadTask<ThreadType::GameThread>( std::move( lambda ) );
			}
		}
	}

	void Material::PostLoadImpl()
	{
		if ( HasAnyMipmapTexture() )
		{
			TextureStreamingManager::GetInstance().Register( *this );
		}

		AddProperty( "MaterialId", m_materialId );

		for ( uint32 i = 0; i < agl::NumNonRTShaderTypes<uint32>; ++i )
		{
			auto shader = m_shaders[i].get();
			if ( shader == nullptr )
			{
				continue;
			}

			m_permutation[i] = shader->CreatePermutation();

			for ( const auto& define : m_defines[i] )
			{
				m_permutation[i]->SetValueByName( ShaderDefineNameHash( define.first.CStr() ), define.second );
			}
		}

		m_materialResource = std::make_unique<MaterialResource>();
		m_materialResource->SetMaterial( std::static_pointer_cast<Material>( SharedThis() ) );
	}

	bool Material::HasShaderSource( agl::ShaderType type ) const
	{
		auto index = static_cast<uint32>( type );
		return m_shaders[index].get() != nullptr;
	}

	ShaderBase* Material::GetCompiledShader( agl::ShaderType type, const IShaderPermutation* permutation ) const
	{
		auto index = static_cast<uint32>( type );

		auto& shader = m_shaders[index];
		if ( shader == nullptr )
		{
			return nullptr;
		}

		ShaderPermutation<> empty;
		const IShaderPermutation& defaultPermutation = m_permutation[index].IsValid() ? *m_permutation[index].Get() : empty;
		const IShaderPermutation& compileOption = permutation ? *permutation : defaultPermutation;

		return shader->CompileShader( compileOption );
	}

	bool Material::HasAnyMipmapTexture() const
	{
		for ( const auto& [name, property] : m_properties )
		{
			if ( property->Type() != MaterialPropertyType::Texture )
			{
				continue;
			}

			if ( auto textureProperty = Cast<const TextureProperty>( property.get() ) )
			{
				const Texture* texture = textureProperty->Value().get();
				if ( texture && ( texture->GetMipLevels() > 1 ) )
				{
					return true;
				}
			}
		}

		return false;
	}
}
