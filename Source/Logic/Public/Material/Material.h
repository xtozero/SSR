#pragma once

#include "AssetLoader/AssetFactory.h"
#include "AssetLoader/IAsyncLoadableAsset.h"
#include "common.h"
#include "DDSTexture.h"
#include "GraphicsApiResource.h"
#include "Math/CXMFloat.h"

#include <filesystem>
#include <map>
#include <string>

class Archive;

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
	virtual MaterialPropertyType Type( ) = 0;

	virtual ~IMaterialProperty( ) = default;
};

class MaterialProperty : public IMaterialProperty
{
public:
};

class FloatProperty : public MaterialProperty
{
public:
	virtual void Serialize( Archive& ar );
	virtual MaterialPropertyType Type( ) override
	{
		return MaterialPropertyType::Float;
	}

	float Value( ) const { return m_value; }

	explicit FloatProperty( float value ) : m_value( value ) {}
	FloatProperty( ) = default;

private:
	float m_value;
};

class Float4Property : public MaterialProperty
{
public:
	virtual void Serialize( Archive& ar );
	virtual MaterialPropertyType Type( ) override
	{
		return MaterialPropertyType::Float4;
	}

	const CXMFLOAT4& Value( ) const { return m_value; }

	explicit Float4Property( const CXMFLOAT4& value ) : m_value( value ) {}
	Float4Property( ) = default;

private:
	CXMFLOAT4 m_value;
};

class IntProperty : public MaterialProperty
{
public:
	virtual void Serialize( Archive& ar );
	virtual MaterialPropertyType Type( ) override
	{
		return MaterialPropertyType::Int;
	}

	int Value( ) const { return m_value; }

	explicit IntProperty( int value ) : m_value( value ) {}
	IntProperty( ) = default;

private:
	int m_value;
};

class TextureProperty : public MaterialProperty
{
public:
	virtual void Serialize( Archive& ar );
	virtual MaterialPropertyType Type( ) override
	{
		return MaterialPropertyType::Texture;
	}

	std::shared_ptr<Texture> Value( ) const { return m_value; }

	explicit TextureProperty( const std::shared_ptr<Texture>& value ) : m_value( value ) {}
	TextureProperty( ) = default;

private:
	std::shared_ptr<Texture> m_value;
};

class Material : public AsyncLoadableAsset
{
	DECLARE_ASSET( LOGIC, Material );
public:
	LOGIC_DLL virtual void Serialize( Archive& ar ) override;

	const std::filesystem::path& Path( ) const { return m_path; }
	void SetPath( const std::filesystem::path& path ) { m_path = path; }

	LOGIC_DLL void AddProperty( const char* key, int value );
	LOGIC_DLL void AddProperty( const char* key, float value );
	LOGIC_DLL void AddProperty( const char* key, const CXMFLOAT4& value );
	LOGIC_DLL void AddProperty( const char* key, const std::shared_ptr<Texture>& value );

	int AsInteger( const char* key ) const;
	float AsFloat( const char* key ) const;
	const CXMFLOAT4& AsVector( const char* key ) const;
	aga::Texture* AsTexture( const char* key ) const;

	const MaterialProperty* HasProperty( const char* key ) const;

	const std::string& Name( ) const { return m_name; }

	LOGIC_DLL Material( const char* name ) : m_name( name ) {}
	LOGIC_DLL Material( ) = default;
	Material( const Material& ) = default;
	Material& operator=( const Material& ) = default;
	Material( Material&& ) = default;
	Material& operator=( Material&& ) = default;

protected:
	LOGIC_DLL virtual void PostLoadImpl( ) override;

private:
	std::filesystem::path m_path;

	std::string m_name;
	std::map<std::string, MaterialProperty*> m_properties;
};