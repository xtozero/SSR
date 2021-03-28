#pragma once

#include "AssetLoader/AssetFactory.h"
#include "AssetLoader/IAsyncLoadableAsset.h"
#include "ShaderPrameterInfo.h"
#include "ShaderPrameterMap.h"
#include "ShaderResource.h"

class ShaderBase : public AsyncLoadableAsset
{
public:
	RENDERCORE_DLL virtual void Serialize( Archive& ar ) override;

	ShaderBase( BinaryChunk&& byteCode ) : m_byteCode( std::move( byteCode ) ) {}
	ShaderBase( ) = default;

	friend bool operator==( const ShaderBase& lhs, const ShaderBase& rhs )
	{
		return lhs.m_byteCode == rhs.m_byteCode;
	}

	ShaderParameterMap& ParameterMap( ) { return m_parameterMap; }
	const ShaderParameterMap& ParameterMap( ) const { return m_parameterMap; }

	ShaderParameterInfo& ParameterInfo( ) { return m_parameterInfo; }
	const ShaderParameterInfo& ParameterInfo( ) const { return m_parameterInfo; }

protected:
	BinaryChunk m_byteCode{ 0 };
	ShaderParameterMap m_parameterMap;
	ShaderParameterInfo m_parameterInfo;
};

class VertexShader : public ShaderBase
{
	DECLARE_ASSET( RENDERCORE, VertexShader );
public:
	RENDERCORE_DLL virtual void Serialize( Archive& ar ) override;

	const std::filesystem::path& Path( ) const { return m_path; }
	void SetPath( const std::filesystem::path& path ) { m_path = path; }

	bool IsValid( ) const
	{
		return m_shader.Get( ) != nullptr;
	}

	VertexShader( BinaryChunk&& byteCode ) : ShaderBase( std::move( byteCode ) ) {}
	VertexShader( ) = default;

	operator aga::VertexShader*( )
	{
		return m_shader.Get( );
	}

	operator const aga::VertexShader*( ) const
	{
		return m_shader.Get( );
	}

	friend bool operator==( const VertexShader& lhs, const VertexShader& rhs )
	{
		return static_cast<const ShaderBase&>( lhs ) == static_cast<const ShaderBase&>( rhs )
			&& lhs.m_shader == rhs.m_shader;
	}

protected:
	RENDERCORE_DLL virtual void PostLoadImpl( ) override;

private:
	std::filesystem::path m_path;
	RefHandle<aga::VertexShader> m_shader;
};

class PixelShader : public ShaderBase
{
	DECLARE_ASSET( RENDERCORE, PixelShader );
public:
	RENDERCORE_DLL virtual void Serialize( Archive& ar ) override;

	const std::filesystem::path& Path( ) const { return m_path; }
	void SetPath( const std::filesystem::path& path ) { m_path = path; }

	bool IsValid( ) const
	{
		return m_shader.Get( ) != nullptr;
	}

	PixelShader( BinaryChunk&& byteCode ) : ShaderBase( std::move( byteCode ) ) {}
	PixelShader( ) = default;

	operator aga::PixelShader*( )
	{
		return m_shader.Get( );
	}

	operator const aga::PixelShader*( ) const
	{
		return m_shader.Get( );
	}

	friend bool operator==( const PixelShader& lhs, const PixelShader& rhs )
	{
		return static_cast<const ShaderBase&>( lhs ) == static_cast<const ShaderBase&>( rhs )
			&& lhs.m_shader == rhs.m_shader;
	}

protected:
	RENDERCORE_DLL virtual void PostLoadImpl( ) override;

private:
	std::filesystem::path m_path;
	RefHandle<aga::PixelShader> m_shader;
};

class ComputeShader : public ShaderBase
{
	DECLARE_ASSET( RENDERCORE, ComputeShader );
public:
	RENDERCORE_DLL virtual void Serialize( Archive& ar ) override;

	const std::filesystem::path& Path( ) const { return m_path; }
	void SetPath( const std::filesystem::path& path ) { m_path = path; }

	ComputeShader( BinaryChunk&& byteCode ) : ShaderBase( std::move( byteCode ) ) {}
	ComputeShader( ) = default;

	operator aga::ComputeShader*( )
	{
		return m_shader.Get( );
	}

	operator const aga::ComputeShader*( ) const
	{
		return m_shader.Get( );
	}

	friend bool operator==( const ComputeShader& lhs, const ComputeShader& rhs )
	{
		return static_cast<const ShaderBase&>( lhs ) == static_cast<const ShaderBase&>( rhs )
			&& lhs.m_shader == rhs.m_shader;
	}

protected:
	RENDERCORE_DLL virtual void PostLoadImpl( ) override;

private:
	std::filesystem::path m_path;
	RefHandle<aga::ComputeShader> m_shader;
};
