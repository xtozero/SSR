#pragma once

#include "AssetFactory.h"
#include "IAsyncLoadableAsset.h"
#include "ShaderParameterInfo.h"
#include "ShaderParameterMap.h"
#include "ShaderRegistry.h"
#include "ShaderResource.h"

namespace rendercore
{
	class IShaderPermutation;
	class ShaderBase;

	class ShaderAsset : public AsyncLoadableAsset
	{
		GENERATE_CLASS_TYPE_INFO( ShaderAsset );

	public:
		RENDERCORE_DLL ShaderPermutationInstance CreatePermutation() const;

		RENDERCORE_DLL virtual ShaderBase* CompileShader( const IShaderPermutation& permutation ) = 0;
		RENDERCORE_DLL virtual void RecompileShader() = 0;

		RENDERCORE_DLL virtual agl::ShaderParameterMap& ParameterMap() = 0;
		RENDERCORE_DLL virtual const agl::ShaderParameterMap& ParameterMap() const = 0;

		RENDERCORE_DLL virtual agl::ShaderParameterInfo& ParameterInfo() = 0;
		RENDERCORE_DLL virtual const agl::ShaderParameterInfo& ParameterInfo() const = 0;

		RENDERCORE_DLL ShaderAsset();
		RENDERCORE_DLL virtual ~ShaderAsset() override;

	protected:
		ShaderPermutationCreateFunc m_createPermutationFunc = nullptr;
	};

	class ShaderBase : public ShaderAsset
	{
		GENERATE_CLASS_TYPE_INFO( ShaderBase );

	public:
		RENDERCORE_DLL virtual ShaderBase* CompileShader( const IShaderPermutation& permutation ) override;
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
		void SetPermutationCreateFunc( ShaderPermutationCreateFunc createFunc );

		ShaderBase( BinaryChunk&& byteCode, size_t hash, uint32 permutationId )
			: m_byteCode( std::move( byteCode ) )
			, m_hash( hash )
			, m_permutationId( permutationId )
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

		PROPERTY( permutationId )
		uint32 m_permutationId = 0;

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

		VertexShader( BinaryChunk&& byteCode, size_t hash, uint32 permutationId )
			: ShaderBase( std::move( byteCode ), hash, permutationId ) {}
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

		GeometryShader( BinaryChunk&& byteCode, size_t hash, uint32 permutationId )
			: ShaderBase( std::move( byteCode ), hash, permutationId ) {}
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

		PixelShader( BinaryChunk&& byteCode, size_t hash, uint32 permutationId )
			: ShaderBase( std::move( byteCode ), hash, permutationId ) {}
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

		ComputeShader( BinaryChunk&& byteCode, size_t hash, uint32 permutationId )
			: ShaderBase( std::move( byteCode ), hash, permutationId ) {}
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

		RENDERCORE_DLL virtual ShaderBase* CompileShader( const IShaderPermutation& permutation ) override;

		MeshShader( BinaryChunk&& byteCode, size_t hash, uint32 permutationId )
			: ShaderBase( std::move( byteCode ), hash, permutationId ) {}
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

		RENDERCORE_DLL virtual ShaderBase* CompileShader( const IShaderPermutation& permutation ) override;

		AmplificationShader( BinaryChunk&& byteCode, size_t hash, uint32 permutationId )
			: ShaderBase( std::move( byteCode ), hash, permutationId ) {}
		AmplificationShader() = default;

		agl::AmplificationShader* Resource();
		const agl::AmplificationShader* Resource() const;

		RENDERCORE_DLL virtual void CreateShader() override;
	};

	class RaytracingShaderBase : public ShaderBase
	{
		GENERATE_CLASS_TYPE_INFO( RaytracingShaderBase );

	public:
		RaytracingShaderBase( BinaryChunk&& byteCode, size_t hash, uint32 permutationId, Name exportName )
			: ShaderBase( std::move( byteCode ), hash, permutationId )
			, m_exportName( exportName ) {}
		RaytracingShaderBase() = default;

	protected:
		PROPERTY( exportName )
		Name m_exportName;
	};

	class RayGenerationShader final : public RaytracingShaderBase
	{
		GENERATE_CLASS_TYPE_INFO( RayGenerationShader );
		DECLARE_ASSET( RENDERCORE, RayGenerationShader );

	public:
		static constexpr agl::ShaderType Type = agl::ShaderType::RayGen;

		RENDERCORE_DLL virtual ShaderBase* CompileShader( const IShaderPermutation& permutation ) override;

		RayGenerationShader( BinaryChunk&& byteCode, size_t hash, uint32 permutationId, Name exportName )
			: RaytracingShaderBase( std::move( byteCode ), hash, permutationId, exportName ) {}
		RayGenerationShader() = default;

		agl::RayGenerationShader* Resource();
		const agl::RayGenerationShader* Resource() const;

		RENDERCORE_DLL virtual void CreateShader() override;
	};

	class IntersectionShader final : public RaytracingShaderBase
	{
		GENERATE_CLASS_TYPE_INFO( IntersectionShader );
		DECLARE_ASSET( RENDERCORE, IntersectionShader );

	public:
		static constexpr agl::ShaderType Type = agl::ShaderType::Intersection;

		RENDERCORE_DLL virtual ShaderBase* CompileShader( const IShaderPermutation& permutation ) override;

		IntersectionShader( BinaryChunk&& byteCode, size_t hash, uint32 permutationId, Name exportName )
			: RaytracingShaderBase( std::move( byteCode ), hash, permutationId, exportName ) {}
		IntersectionShader() = default;

		agl::IntersectionShader* Resource();
		const agl::IntersectionShader* Resource() const;

		RENDERCORE_DLL virtual void CreateShader() override;
	};

	class AnyHitShader final : public RaytracingShaderBase
	{
		GENERATE_CLASS_TYPE_INFO( AnyHitShader );
		DECLARE_ASSET( RENDERCORE, AnyHitShader );

	public:
		static constexpr agl::ShaderType Type = agl::ShaderType::AnyHit;

		RENDERCORE_DLL virtual ShaderBase* CompileShader( const IShaderPermutation& permutation ) override;

		AnyHitShader( BinaryChunk&& byteCode, size_t hash, uint32 permutationId, Name exportName )
			: RaytracingShaderBase( std::move( byteCode ), hash, permutationId, exportName ) {}
		AnyHitShader() = default;

		agl::AnyHitShader* Resource();
		const agl::AnyHitShader* Resource() const;

		RENDERCORE_DLL virtual void CreateShader() override;
	};

	class ClosestHitShader final : public RaytracingShaderBase
	{
		GENERATE_CLASS_TYPE_INFO( ClosestHitShader );
		DECLARE_ASSET( RENDERCORE, ClosestHitShader );

	public:
		static constexpr agl::ShaderType Type = agl::ShaderType::ClosestHit;

		RENDERCORE_DLL virtual ShaderBase* CompileShader( const IShaderPermutation& permutation ) override;

		ClosestHitShader( BinaryChunk&& byteCode, size_t hash, uint32 permutationId, Name exportName )
			: RaytracingShaderBase( std::move( byteCode ), hash, permutationId, exportName ) {}
		ClosestHitShader() = default;

		agl::ClosestHitShader* Resource();
		const agl::ClosestHitShader* Resource() const;

		RENDERCORE_DLL virtual void CreateShader() override;
	};

	class MissShader final : public RaytracingShaderBase
	{
		GENERATE_CLASS_TYPE_INFO( MissShader );
		DECLARE_ASSET( RENDERCORE, MissShader );

	public:
		static constexpr agl::ShaderType Type = agl::ShaderType::Miss;

		RENDERCORE_DLL virtual ShaderBase* CompileShader( const IShaderPermutation& permutation ) override;

		MissShader( BinaryChunk&& byteCode, size_t hash, uint32 permutationId, Name exportName )
			: RaytracingShaderBase( std::move( byteCode ), hash, permutationId, exportName ) {}
		MissShader() = default;

		agl::MissShader* Resource();
		const agl::MissShader* Resource() const;

		RENDERCORE_DLL virtual void CreateShader() override;
	};

	class CallableShader final : public RaytracingShaderBase
	{
		GENERATE_CLASS_TYPE_INFO( CallableShader );
		DECLARE_ASSET( RENDERCORE, CallableShader );

	public:
		static constexpr agl::ShaderType Type = agl::ShaderType::Callable;

		RENDERCORE_DLL virtual ShaderBase* CompileShader( const IShaderPermutation& permutation ) override;

		CallableShader( BinaryChunk&& byteCode, size_t hash, uint32 permutationId, Name exportName )
			: RaytracingShaderBase( std::move( byteCode ), hash, permutationId, exportName ) {}
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
