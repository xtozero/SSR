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

	class ShaderAsset : public AsyncLoadableAsset
	{
		GENERATE_CLASS_TYPE_INFO( ShaderAsset );

	public:
		RENDERCORE_DLL StaticShaderSwitches GetStaticSwitches() const;

		RENDERCORE_DLL virtual ShaderBase* CompileShader( const StaticShaderSwitches& switches ) = 0;
		RENDERCORE_DLL virtual void RecompileShader() = 0;

		RENDERCORE_DLL virtual agl::ShaderParameterMap& ParameterMap() = 0;
		RENDERCORE_DLL virtual const agl::ShaderParameterMap& ParameterMap() const = 0;

		RENDERCORE_DLL virtual agl::ShaderParameterInfo& ParameterInfo() = 0;
		RENDERCORE_DLL virtual const agl::ShaderParameterInfo& ParameterInfo() const = 0;

		explicit ShaderAsset( const StaticShaderSwitches& switches );
		RENDERCORE_DLL ShaderAsset();
		RENDERCORE_DLL virtual ~ShaderAsset() override;

	protected:
		PROPERTY( switches )
		StaticShaderSwitches m_switches;
	};

	class ShaderBase : public ShaderAsset
	{
		GENERATE_CLASS_TYPE_INFO( ShaderBase );

	public:
		RENDERCORE_DLL virtual ShaderBase* CompileShader( const StaticShaderSwitches& switches ) override;
		RENDERCORE_DLL virtual void RecompileShader() override;

		bool IsValid() const
		{
			return m_shader.Get() != nullptr;
		}

		void SetHash( size_t hash );
		size_t GetHash() const;

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

		static void ReloadShaders();

		ShaderAsset* GetParent();
		const ShaderAsset* GetParent() const;
		void SetParent( const std::shared_ptr<ShaderAsset>& parent );

		ShaderBase( const StaticShaderSwitches& switches, BinaryChunk&& byteCode, size_t hash )
			: Super( switches )
			, m_byteCode( std::move( byteCode ) )
			, m_hash( hash )
		{}
		ShaderBase() = default;

		friend bool operator==( const ShaderBase& lhs, const ShaderBase& rhs )
		{
			return lhs.m_shader == rhs.m_shader
				|| lhs.m_byteCode == rhs.m_byteCode;
		}

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

		std::shared_ptr<ShaderAsset> m_parent = nullptr;
		RefHandle<agl::Shader> m_shader;

	private:
		void RecreateShader();
	};

	class VertexShader final : public ShaderBase
	{
		GENERATE_CLASS_TYPE_INFO( VertexShader );
		DECLARE_ASSET( RENDERCORE, VertexShader );

	public:
		static constexpr agl::ShaderType Type = agl::ShaderType::Vertex;

		VertexShader( const StaticShaderSwitches& switches, BinaryChunk&& byteCode, size_t hash )
			: ShaderBase( switches, std::move( byteCode ), hash ) {}
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
		static constexpr agl::ShaderType Type = agl::ShaderType::Geometry;

		GeometryShader( const StaticShaderSwitches& switches, BinaryChunk&& byteCode, size_t hash )
			: ShaderBase( switches, std::move( byteCode ), hash ) {}
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
		static constexpr agl::ShaderType Type = agl::ShaderType::Pixel;

		PixelShader( const StaticShaderSwitches& switches, BinaryChunk&& byteCode, size_t hash )
			: ShaderBase( switches, std::move( byteCode ), hash ) {}
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
		static constexpr agl::ShaderType Type = agl::ShaderType::Compute;

		ComputeShader( const StaticShaderSwitches& switches, BinaryChunk&& byteCode, size_t hash )
			: ShaderBase( switches, std::move( byteCode ), hash ) {}
		ComputeShader() = default;

		agl::ComputeShader* Resource();
		const agl::ComputeShader* Resource() const;

		RENDERCORE_DLL virtual void CreateShader() override;
	};

	class MeshShader final : public ShaderBase
	{
		GENERATE_CLASS_TYPE_INFO( MeshShader );
		DECLARE_ASSET( RENDERCORE, MeshShader );

	public:
		static constexpr agl::ShaderType Type = agl::ShaderType::Mesh;

		RENDERCORE_DLL virtual ShaderBase* CompileShader( const StaticShaderSwitches& switches ) override;

		MeshShader( const StaticShaderSwitches& switches, BinaryChunk&& byteCode, size_t hash )
			: ShaderBase( switches, std::move( byteCode ), hash ) {}
		MeshShader() = default;

		agl::MeshShader* Resource();
		const agl::MeshShader* Resource() const;

		RENDERCORE_DLL virtual void CreateShader() override;
	};

	class AmplificationShader final : public ShaderBase
	{
		GENERATE_CLASS_TYPE_INFO( AmplificationShader );
		DECLARE_ASSET( RENDERCORE, AmplificationShader );

	public:
		static constexpr agl::ShaderType Type = agl::ShaderType::Amplification;

		RENDERCORE_DLL virtual ShaderBase* CompileShader( const StaticShaderSwitches& switches ) override;

		AmplificationShader( const StaticShaderSwitches& switches, BinaryChunk&& byteCode, size_t hash )
			: ShaderBase( switches, std::move( byteCode ), hash ) {}
		AmplificationShader() = default;

		agl::AmplificationShader* Resource();
		const agl::AmplificationShader* Resource() const;

		RENDERCORE_DLL virtual void CreateShader() override;
	};

	class RayGenerationShader final : public ShaderBase
	{
		GENERATE_CLASS_TYPE_INFO( RayGenerationShader );
		DECLARE_ASSET( RENDERCORE, RayGenerationShader );

	public:
		static constexpr agl::ShaderType Type = agl::ShaderType::RayGen;

		RENDERCORE_DLL virtual ShaderBase* CompileShader( const StaticShaderSwitches& switches ) override;

		RayGenerationShader( const StaticShaderSwitches& switches, BinaryChunk&& byteCode, size_t hash )
			: ShaderBase( switches, std::move( byteCode ), hash ) {}
		RayGenerationShader() = default;

		agl::RayGenerationShader* Resource();
		const agl::RayGenerationShader* Resource() const;

		RENDERCORE_DLL virtual void CreateShader() override;
	};

	class IntersectionShader final : public ShaderBase
	{
		GENERATE_CLASS_TYPE_INFO( IntersectionShader );
		DECLARE_ASSET( RENDERCORE, IntersectionShader );

	public:
		static constexpr agl::ShaderType Type = agl::ShaderType::Intersection;

		RENDERCORE_DLL virtual ShaderBase* CompileShader( const StaticShaderSwitches& switches ) override;

		IntersectionShader( const StaticShaderSwitches& switches, BinaryChunk&& byteCode, size_t hash )
			: ShaderBase( switches, std::move( byteCode ), hash ) {}
		IntersectionShader() = default;

		agl::IntersectionShader* Resource();
		const agl::IntersectionShader* Resource() const;

		RENDERCORE_DLL virtual void CreateShader() override;
	};

	class AnyHitShader final : public ShaderBase
	{
		GENERATE_CLASS_TYPE_INFO( AnyHitShader );
		DECLARE_ASSET( RENDERCORE, AnyHitShader );

	public:
		static constexpr agl::ShaderType Type = agl::ShaderType::AnyHit;

		RENDERCORE_DLL virtual ShaderBase* CompileShader( const StaticShaderSwitches& switches ) override;

		AnyHitShader( const StaticShaderSwitches& switches, BinaryChunk&& byteCode, size_t hash )
			: ShaderBase( switches, std::move( byteCode ), hash ) {}
		AnyHitShader() = default;

		agl::AnyHitShader* Resource();
		const agl::AnyHitShader* Resource() const;

		RENDERCORE_DLL virtual void CreateShader() override;
	};

	class ClosestHitShader final : public ShaderBase
	{
		GENERATE_CLASS_TYPE_INFO( ClosestHitShader );
		DECLARE_ASSET( RENDERCORE, ClosestHitShader );

	public:
		static constexpr agl::ShaderType Type = agl::ShaderType::ClosestHit;

		RENDERCORE_DLL virtual ShaderBase* CompileShader( const StaticShaderSwitches& switches ) override;

		ClosestHitShader( const StaticShaderSwitches& switches, BinaryChunk&& byteCode, size_t hash )
			: ShaderBase( switches, std::move( byteCode ), hash ) {}
		ClosestHitShader() = default;

		agl::ClosestHitShader* Resource();
		const agl::ClosestHitShader* Resource() const;

		RENDERCORE_DLL virtual void CreateShader() override;
	};

	class MissShader final : public ShaderBase
	{
		GENERATE_CLASS_TYPE_INFO( MissShader );
		DECLARE_ASSET( RENDERCORE, MissShader );

	public:
		static constexpr agl::ShaderType Type = agl::ShaderType::Miss;

		RENDERCORE_DLL virtual ShaderBase* CompileShader( const StaticShaderSwitches& switches ) override;

		MissShader( const StaticShaderSwitches& switches, BinaryChunk&& byteCode, size_t hash )
			: ShaderBase( switches, std::move( byteCode ), hash ) {}
		MissShader() = default;

		agl::MissShader* Resource();
		const agl::MissShader* Resource() const;

		RENDERCORE_DLL virtual void CreateShader() override;
	};

	class CallableShader final : public ShaderBase
	{
		GENERATE_CLASS_TYPE_INFO( CallableShader );
		DECLARE_ASSET( RENDERCORE, CallableShader );

	public:
		static constexpr agl::ShaderType Type = agl::ShaderType::Callable;

		RENDERCORE_DLL virtual ShaderBase* CompileShader( const StaticShaderSwitches& switches ) override;

		CallableShader( const StaticShaderSwitches& switches, BinaryChunk&& byteCode, size_t hash )
			: ShaderBase( switches, std::move( byteCode ), hash ) {}
		CallableShader() = default;

		agl::CallableShader* Resource();
		const agl::CallableShader* Resource() const;

		RENDERCORE_DLL virtual void CreateShader() override;
	};

	template <typename ShaderClass>
	class ShaderTraits
	{
	public:
		static constexpr agl::ShaderType Type = ShaderClass::Type;
	};

	template <typename ShaderClass>
	concept HasShaderType = requires
	{
		{ ShaderClass::Type } -> std::convertible_to<agl::ShaderType>;
	};
}
