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

		bool IsValid() const
		{
			return m_shader.Get() != nullptr;
		}

		void SetHash( size_t hash );
		size_t GetHash() const;

		ShaderBase( BinaryChunk&& byteCode, size_t hash ) 
			: m_byteCode( std::move( byteCode ) )
			, m_hash( hash )
		{}
		ShaderBase() = default;

		friend bool operator==( const ShaderBase& lhs, const ShaderBase& rhs )
		{
			return lhs.m_shader == rhs.m_shader
				|| lhs.m_byteCode == rhs.m_byteCode;
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

		PROPERTY( hash )
		size_t m_hash = 0;

		RefHandle<agl::GraphicsApiResource> m_shader;
	};

	class VertexShader final : public ShaderBase
	{
		GENERATE_CLASS_TYPE_INFO( VertexShader );
		DECLARE_ASSET( RENDERCORE, VertexShader );

	public:
		VertexShader( BinaryChunk&& byteCode, size_t hash ) : ShaderBase( std::move( byteCode ), hash ) {}
		VertexShader() = default;

		agl::VertexShader* Resource();
		const agl::VertexShader* Resource() const;

		RENDERCORE_DLL virtual void CreateShader() override;
	};

	class GeometryShader final : public ShaderBase
	{
		GENERATE_CLASS_TYPE_INFO( GeometryShader );
		DECLARE_ASSET( RENDERCORE, GeometryShader );

	public:
		GeometryShader( BinaryChunk&& byteCode, size_t hash ) : ShaderBase( std::move( byteCode ), hash ) {}
		GeometryShader() = default;

		agl::GeometryShader* Resource();
		const agl::GeometryShader* Resource() const;

		RENDERCORE_DLL virtual void CreateShader() override;
	};

	class PixelShader final : public ShaderBase
	{
		GENERATE_CLASS_TYPE_INFO( PixelShader );
		DECLARE_ASSET( RENDERCORE, PixelShader );

	public:
		PixelShader( BinaryChunk&& byteCode, size_t hash ) : ShaderBase( std::move( byteCode ), hash ) {}
		PixelShader() = default;

		agl::PixelShader* Resource();
		const agl::PixelShader* Resource() const;

		RENDERCORE_DLL virtual void CreateShader() override;
	};

	class ComputeShader final : public ShaderBase
	{
		GENERATE_CLASS_TYPE_INFO( ComputeShader );
		DECLARE_ASSET( RENDERCORE, ComputeShader );

	public:
		ComputeShader( BinaryChunk&& byteCode, size_t hash ) : ShaderBase( std::move( byteCode ), hash ) {}
		ComputeShader() = default;

		agl::ComputeShader* Resource();
		const agl::ComputeShader* Resource() const;

		RENDERCORE_DLL virtual void CreateShader() override;
	};
}
