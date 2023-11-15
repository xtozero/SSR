#pragma once

#include "AssetFactory.h"
#include "IAsyncLoadableAsset.h"
#include "ShaderParameterInfo.h"
#include "ShaderParameterMap.h"
#include "ShaderResource.h"
#include "StaticShaderSwitch.h"

namespace rendercore
{
	class ShaderBase;

	class IShader : public AsyncLoadableAsset
	{
		GENERATE_CLASS_TYPE_INFO( IShader );

	public:
		RENDERCORE_DLL virtual StaticShaderSwitches GetStaticSwitches() const = 0;
		RENDERCORE_DLL virtual ShaderBase* CompileShader( const StaticShaderSwitches& switches ) = 0;

		RENDERCORE_DLL virtual agl::ShaderParameterMap& ParameterMap() = 0;
		RENDERCORE_DLL virtual const agl::ShaderParameterMap& ParameterMap() const = 0;

		RENDERCORE_DLL virtual agl::ShaderParameterInfo& ParameterInfo() = 0;
		RENDERCORE_DLL virtual const agl::ShaderParameterInfo& ParameterInfo() const = 0;
	};

	class ShaderBase : public IShader
	{
		GENERATE_CLASS_TYPE_INFO( ShaderBase );

	public:
		RENDERCORE_DLL virtual StaticShaderSwitches GetStaticSwitches() const override;
		RENDERCORE_DLL virtual ShaderBase* CompileShader( const StaticShaderSwitches& switches ) override;

		ShaderBase( BinaryChunk&& byteCode ) : m_byteCode( std::move( byteCode ) ) {}
		ShaderBase() = default;

		friend bool operator==( const ShaderBase& lhs, const ShaderBase& rhs )
		{
			return lhs.m_byteCode == rhs.m_byteCode;
		}

		friend Archive& operator<<( Archive& ar, ShaderBase& shaderBase );

		const BinaryChunk& ByteCode() const
		{
			return m_byteCode;
		}

		RENDERCORE_DLL virtual agl::ShaderParameterMap& ParameterMap() override
		{
			return m_parameterMap;
		}
		RENDERCORE_DLL virtual const agl::ShaderParameterMap& ParameterMap() const override
		{
			return m_parameterMap;
		}

		RENDERCORE_DLL virtual agl::ShaderParameterInfo& ParameterInfo() override
		{
			return m_parameterInfo;
		}
		RENDERCORE_DLL virtual const agl::ShaderParameterInfo& ParameterInfo() const override
		{
			return m_parameterInfo;
		}

		RENDERCORE_DLL virtual void CreateShader() = 0;

	protected:
		RENDERCORE_DLL virtual void PostLoadImpl() override;

		PROPERTY( byteCode )
		BinaryChunk m_byteCode { 0 };

		PROPERTY( parameterMap )
		agl::ShaderParameterMap m_parameterMap;

		PROPERTY( parameterInfo )
		agl::ShaderParameterInfo m_parameterInfo;
	};

	class VertexShader final : public ShaderBase
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

		agl::VertexShader* Resource();
		const agl::VertexShader* Resource() const;

		friend bool operator==( const VertexShader& lhs, const VertexShader& rhs )
		{
			return static_cast<const ShaderBase&>( lhs ) == static_cast<const ShaderBase&>( rhs )
				&& lhs.m_shader == rhs.m_shader;
		}

		RENDERCORE_DLL virtual void CreateShader() override;

	private:
		agl::RefHandle<agl::VertexShader> m_shader;
	};

	class GeometryShader final : public ShaderBase
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

		agl::GeometryShader* Resource();
		const agl::GeometryShader* Resource() const;

		friend bool operator==( const GeometryShader& lhs, const GeometryShader& rhs )
		{
			return static_cast<const ShaderBase&>( lhs ) == static_cast<const ShaderBase&>( rhs )
				&& lhs.m_shader == rhs.m_shader;
		}

		RENDERCORE_DLL virtual void CreateShader() override;

	private:
		agl::RefHandle<agl::GeometryShader> m_shader;
	};

	class PixelShader final : public ShaderBase
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

		agl::PixelShader* Resource();
		const agl::PixelShader* Resource() const;

		friend bool operator==( const PixelShader& lhs, const PixelShader& rhs )
		{
			return static_cast<const ShaderBase&>( lhs ) == static_cast<const ShaderBase&>( rhs )
				&& lhs.m_shader == rhs.m_shader;
		}

		RENDERCORE_DLL virtual void CreateShader() override;

	private:
		agl::RefHandle<agl::PixelShader> m_shader;
	};

	class ComputeShader final : public ShaderBase
	{
		GENERATE_CLASS_TYPE_INFO( ComputeShader );
		DECLARE_ASSET( RENDERCORE, ComputeShader );

	public:
		bool IsValid() const
		{
			return m_shader.Get() != nullptr;
		}

		ComputeShader( BinaryChunk&& byteCode ) : ShaderBase( std::move( byteCode ) ) {}
		ComputeShader() = default;

		agl::ComputeShader* Resource();
		const agl::ComputeShader* Resource() const;

		friend bool operator==( const ComputeShader& lhs, const ComputeShader& rhs )
		{
			return static_cast<const ShaderBase&>( lhs ) == static_cast<const ShaderBase&>( rhs )
				&& lhs.m_shader == rhs.m_shader;
		}

		RENDERCORE_DLL virtual void CreateShader() override;

	private:
		agl::RefHandle<agl::ComputeShader> m_shader;
	};
}
