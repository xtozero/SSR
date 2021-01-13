#pragma once

#include "AssetLoader/AssetFactory.h"
#include "Core/IAsyncLoadableAsset.h"
#include "ShaderResource.h"

class ShaderBase : public AsyncLoadableAsset
{
public:
	LOGIC_DLL virtual void Serialize( Archive& ar ) override;

	ShaderBase( BinaryChunk&& byteCode ) : m_byteCode( std::move( byteCode ) ) {}
	ShaderBase( ) = default;

	friend bool operator==( const ShaderBase& lhs, const ShaderBase& rhs )
	{
		return lhs.m_byteCode == rhs.m_byteCode;
	}
protected:
	BinaryChunk m_byteCode{ 0 };
};

class VertexShader : public ShaderBase
{
	DECLARE_ASSET( VertexShader );
public:
	LOGIC_DLL virtual void Serialize( Archive& ar ) override;

	const std::filesystem::path& Path( ) const { return m_path; }
	void SetPath( const std::filesystem::path& path ) { m_path = path; }

	VertexShader( BinaryChunk&& byteCode ) : ShaderBase( std::move( byteCode ) ) {}
	VertexShader( ) = default;

	friend bool operator==( const VertexShader& lhs, const VertexShader& rhs )
	{
		return static_cast<const ShaderBase&>( lhs ) == static_cast<const ShaderBase&>( rhs )
			&& lhs.m_shader == rhs.m_shader;
	}
private:
	std::filesystem::path m_path;
	RefHandle<aga::VertexShader> m_shader;
};

class PixelShader : public ShaderBase
{
	DECLARE_ASSET( PixelShader );
public:
	LOGIC_DLL virtual void Serialize( Archive& ar ) override;

	const std::filesystem::path& Path( ) const { return m_path; }
	void SetPath( const std::filesystem::path& path ) { m_path = path; }

	PixelShader( BinaryChunk&& byteCode ) : ShaderBase( std::move( byteCode ) ) {}
	PixelShader( ) = default;

	friend bool operator==( const PixelShader& lhs, const PixelShader& rhs )
	{
		return static_cast<const ShaderBase&>( lhs ) == static_cast<const ShaderBase&>( rhs )
			&& lhs.m_shader == rhs.m_shader;
	}
private:
	std::filesystem::path m_path;
	RefHandle<aga::PixelShader> m_shader;
};
