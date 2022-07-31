#pragma once

#include "AssetFactory.h"
#include "IAsyncLoadableAsset.h"
#include "ShaderParameterInfo.h"
#include "ShaderParameterMap.h"
#include "ShaderResource.h"
#include "StaticShaderSwitch.h"

class ShaderBase;

class IShader : public AsyncLoadableAsset
{
	GENERATE_CLASS_TYPE_INFO( IShader );

public:
	RENDERCORE_DLL virtual ShaderBase* CompileShader( const rendercore::StaticShaderSwitches& switches ) = 0;
};

class ShaderBase : public IShader
{
	GENERATE_CLASS_TYPE_INFO( ShaderBase );

public:
	RENDERCORE_DLL virtual ShaderBase* CompileShader( const rendercore::StaticShaderSwitches& switches ) override;

	ShaderBase( BinaryChunk&& byteCode ) : m_byteCode( std::move( byteCode ) ) {}
	ShaderBase() = default;

	friend bool operator==( const ShaderBase& lhs, const ShaderBase& rhs )
	{
		return lhs.m_byteCode == rhs.m_byteCode;
	}

	const BinaryChunk& ByteCode() const
	{
		return m_byteCode;
	}

	aga::ShaderParameterMap& ParameterMap() { return m_parameterMap; }
	const aga::ShaderParameterMap& ParameterMap() const { return m_parameterMap; }

	aga::ShaderParameterInfo& ParameterInfo() { return m_parameterInfo; }
	const aga::ShaderParameterInfo& ParameterInfo() const { return m_parameterInfo; }

	RENDERCORE_DLL virtual void CreateShader() = 0;

protected:
	RENDERCORE_DLL virtual void PostLoadImpl() override;

	PROPERTY( byteCode )
	BinaryChunk m_byteCode{ 0 };

	PROPERTY( parameterMap )
	aga::ShaderParameterMap m_parameterMap;

	PROPERTY( parameterInfo )
	aga::ShaderParameterInfo m_parameterInfo;
};

class VertexShader : public ShaderBase
{
	GENERATE_CLASS_TYPE_INFO( VertexShader );
	DECLARE_ASSET( RENDERCORE, VertexShader );

public:
	bool IsValid() const
	{
		return m_shader.Get() != nullptr;
	}

	VertexShader( BinaryChunk&& byteCode ) : ShaderBase( std::move( byteCode ) ) {}
	VertexShader() = default;

	aga::VertexShader* Resource();
	const aga::VertexShader* Resource() const;

	friend bool operator==( const VertexShader& lhs, const VertexShader& rhs )
	{
		return static_cast<const ShaderBase&>( lhs ) == static_cast<const ShaderBase&>( rhs )
			&& lhs.m_shader == rhs.m_shader;
	}

	RENDERCORE_DLL virtual void CreateShader() override;

private:
	RefHandle<aga::VertexShader> m_shader;
};

class GeometryShader : public ShaderBase
{
	GENERATE_CLASS_TYPE_INFO( GeometryShader );
	DECLARE_ASSET( RENDERCORE, GeometryShader );

public:
	bool IsValid() const
	{
		return m_shader.Get() != nullptr;
	}

	GeometryShader( BinaryChunk&& byteCode ) : ShaderBase( std::move( byteCode ) ) {}
	GeometryShader() = default;

	aga::GeometryShader* Resource();
	const aga::GeometryShader* Resource() const;

	friend bool operator==( const GeometryShader& lhs, const GeometryShader& rhs )
	{
		return static_cast<const ShaderBase&>( lhs ) == static_cast<const ShaderBase&>( rhs )
			&& lhs.m_shader == rhs.m_shader;
	}

	RENDERCORE_DLL virtual void CreateShader() override;

private:
	RefHandle<aga::GeometryShader> m_shader;
};

class PixelShader : public ShaderBase
{
	GENERATE_CLASS_TYPE_INFO( PixelShader );
	DECLARE_ASSET( RENDERCORE, PixelShader );

public:
	bool IsValid() const
	{
		return m_shader.Get() != nullptr;
	}

	PixelShader( BinaryChunk&& byteCode ) : ShaderBase( std::move( byteCode ) ) {}
	PixelShader() = default;

	aga::PixelShader* Resource();
	const aga::PixelShader* Resource() const;

	friend bool operator==( const PixelShader& lhs, const PixelShader& rhs )
	{
		return static_cast<const ShaderBase&>( lhs ) == static_cast<const ShaderBase&>( rhs )
			&& lhs.m_shader == rhs.m_shader;
	}

	RENDERCORE_DLL virtual void CreateShader() override;

private:
	RefHandle<aga::PixelShader> m_shader;
};

class ComputeShader : public ShaderBase
{
	GENERATE_CLASS_TYPE_INFO( ComputeShader );
	DECLARE_ASSET( RENDERCORE, ComputeShader );

public:
	ComputeShader( BinaryChunk&& byteCode ) : ShaderBase( std::move( byteCode ) ) {}
	ComputeShader() = default;

	aga::ComputeShader* Resource();
	const aga::ComputeShader* Resource() const;

	friend bool operator==( const ComputeShader& lhs, const ComputeShader& rhs )
	{
		return static_cast<const ShaderBase&>( lhs ) == static_cast<const ShaderBase&>( rhs )
			&& lhs.m_shader == rhs.m_shader;
	}

	RENDERCORE_DLL virtual void CreateShader() override;

private:
	RefHandle<aga::ComputeShader> m_shader;
};
