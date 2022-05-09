#pragma once

#include "AssetFactory.h"
#include "common.h"
#include "DDSTexture.h"
#include "GraphicsApiResource.h"
#include "IAsyncLoadableAsset.h"
#include "Math/Vector4.h"
#include "NameTypes.h"
#include "RenderOption.h"
#include "SizedTypes.h"

#include <filesystem>
#include <map>
#include <memory>
#include <string>

class Archive;
class MaterialResource;

enum class MaterialPropertyType
{
	Float = 0,
	Float4,
	Int,
	Texture,
};

class IMaterialProperty
{
public:
	virtual void Serialize( Archive& ar ) = 0;
	virtual MaterialPropertyType Type() const = 0;

	virtual void CopyValue( void* dest ) const = 0;

	virtual ~IMaterialProperty() = default;
};

class MaterialProperty : public IMaterialProperty
{
public:
};

class FloatProperty : public MaterialProperty
{
public:
	virtual void Serialize( Archive& ar );
	virtual MaterialPropertyType Type() const override
	{
		return MaterialPropertyType::Float;
	}

	virtual void CopyValue( void* dest ) const override;

	float Value() const { return m_value; }

	explicit FloatProperty( float value ) : m_value( value ) {}
	FloatProperty() = default;

private:
	float m_value;
};

class Float4Property : public MaterialProperty
{
public:
	virtual void Serialize( Archive& ar );
	virtual MaterialPropertyType Type() const override
	{
		return MaterialPropertyType::Float4;
	}

	virtual void CopyValue( void* dest ) const override;

	const Vector4& Value() const { return m_value; }

	explicit Float4Property( const Vector4& value ) : m_value( value ) {}
	Float4Property() = default;

private:
	Vector4 m_value;
};

class IntProperty : public MaterialProperty
{
public:
	virtual void Serialize( Archive& ar );
	virtual MaterialPropertyType Type() const override
	{
		return MaterialPropertyType::Int;
	}

	virtual void CopyValue( void* dest ) const override;

	int32 Value() const { return m_value; }

	explicit IntProperty( int32 value ) : m_value( value ) {}
	IntProperty() = default;

private:
	int32 m_value;
};

class TextureProperty : public MaterialProperty
{
public:
	virtual void Serialize( Archive& ar );
	virtual MaterialPropertyType Type() const override
	{
		return MaterialPropertyType::Texture;
	}

	virtual void CopyValue( void* dest ) const override;

	std::shared_ptr<Texture> Value() const { return m_value; }

	explicit TextureProperty( const std::shared_ptr<Texture>& value ) : m_value( value ) {}
	TextureProperty() = default;

private:
	std::shared_ptr<Texture> m_value;
};

class Material : public AsyncLoadableAsset
{
	GENERATE_CLASS_TYPE_INFO( Material );
	DECLARE_ASSET( RENDERCORE, Material );

public:
	RENDERCORE_DLL virtual void Serialize( Archive& ar ) override;

	RENDERCORE_DLL void AddProperty( const char* key, int32 value );
	RENDERCORE_DLL void AddProperty( const char* key, float value );
	RENDERCORE_DLL void AddProperty( const char* key, const Vector4& value );
	RENDERCORE_DLL void AddProperty( const char* key, const std::shared_ptr<Texture>& value );

	const MaterialProperty* AsProperty( const char* key ) const;
	int32 AsInteger( const char* key ) const;
	float AsFloat( const char* key ) const;
	const Vector4& AsVector( const char* key ) const;
	Texture* AsTexture( const char* key ) const;
	SamplerOption* AsSampelrOption( const char* key ) const;

	void CopyProperty( const char* key, void* dest ) const;

	bool HasProperty( const char* key ) const;

	const std::string& GetName() const { return m_name; }

	const ShaderBase* GetShader( SHADER_TYPE type ) const;

	RENDERCORE_DLL void SetVertexShader( const std::shared_ptr<VertexShader>& vertexshader );
	const VertexShader* GetVertexShader() const;
	VertexShader* GetVertexShader();
	RENDERCORE_DLL void SetGeometryShader( const std::shared_ptr<GeometryShader>& geometryShader );
	const GeometryShader* GetGeometryShader() const;
	GeometryShader* GetGeometryShader();
	RENDERCORE_DLL void SetPixelShader( const std::shared_ptr<PixelShader>& pixelShader );
	const PixelShader* GetPixelShader() const;
	PixelShader* GetPixelShader();
	RENDERCORE_DLL void AddSampler( const std::string& key, const  std::shared_ptr<SamplerOption>& samplerOption );

	RENDERCORE_DLL MaterialResource* GetMaterialResource() const;

	RENDERCORE_DLL Material( const char* name );
	RENDERCORE_DLL Material();
	RENDERCORE_DLL ~Material();
	Material( const Material& ) = delete;
	Material& operator=( const Material& ) = delete;
	RENDERCORE_DLL Material( Material&& );
	RENDERCORE_DLL Material& operator=( Material&& );

protected:
	RENDERCORE_DLL virtual void PostLoadImpl() override;

private:
	std::string m_name;
	std::shared_ptr<VertexShader> m_vertexShader = nullptr;
	std::shared_ptr<GeometryShader> m_geometryShader = nullptr;
	std::shared_ptr<PixelShader> m_pixelShader = nullptr;
	std::map<Name, std::unique_ptr<MaterialProperty>> m_properties;
	std::map<Name, std::shared_ptr<SamplerOption>> m_samplers;

	std::unique_ptr<MaterialResource> m_materialResource;
};