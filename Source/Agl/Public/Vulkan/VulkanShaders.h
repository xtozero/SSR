#pragma once

namespace agl
{
	class VulkanVertexShader final : public VertexShader
	{
	public:
		VulkanVertexShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo )
			: VertexShader( byteCode, byteCodeSize, paramInfo ) {}
		VulkanVertexShader( const VulkanVertexShader& ) = delete;
		VulkanVertexShader( VulkanVertexShader&& ) = default;
		VulkanVertexShader& operator=( const VulkanVertexShader& ) = delete;
		VulkanVertexShader& operator=( VulkanVertexShader&& ) = default;
		virtual ~VulkanVertexShader() override = default;

	private:
		virtual void InitResource() override {}
		virtual void FreeResource() override {}
	};

	class VulkanGeometryShader final : public GeometryShader
	{
	public:
		VulkanGeometryShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo )
			: GeometryShader( byteCode, byteCodeSize, paramInfo ) {}
		VulkanGeometryShader( const VulkanGeometryShader& ) = delete;
		VulkanGeometryShader( VulkanGeometryShader&& ) = default;
		VulkanGeometryShader& operator=( const VulkanGeometryShader& ) = delete;
		VulkanGeometryShader& operator=( VulkanGeometryShader&& ) = default;
		virtual ~VulkanGeometryShader() override = default;

	private:
		virtual void InitResource() override {}
		virtual void FreeResource() override {}
	};

	class VulkanPixelShader final : public PixelShader
	{
	public:
		VulkanPixelShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo )
			: PixelShader( byteCode, byteCodeSize, paramInfo ) {}
		VulkanPixelShader( const VulkanPixelShader& ) = delete;
		VulkanPixelShader( VulkanPixelShader&& ) = default;
		VulkanPixelShader& operator=( const VulkanPixelShader& ) = delete;
		VulkanPixelShader& operator=( VulkanPixelShader&& ) = default;
		virtual ~VulkanPixelShader() override = default;

	private:
		virtual void InitResource() override {}
		virtual void FreeResource() override {}
	};

	class VulkanComputeShader final : public ComputeShader
	{
	public:
		VulkanComputeShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo )
			: ComputeShader( byteCode, byteCodeSize, paramInfo ) {}
		VulkanComputeShader( const VulkanComputeShader& ) = delete;
		VulkanComputeShader( VulkanComputeShader&& ) = default;
		VulkanComputeShader& operator=( const VulkanComputeShader& ) = delete;
		VulkanComputeShader& operator=( VulkanComputeShader&& ) = default;
		virtual ~VulkanComputeShader() override = default;

	private:
		virtual void InitResource() override {}
		virtual void FreeResource() override {}
	};

	class VulkanMeshShader final : public MeshShader
	{
	public:
		VulkanMeshShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo )
			: MeshShader( byteCode, byteCodeSize, paramInfo ) {}
		VulkanMeshShader( const VulkanMeshShader& ) = delete;
		VulkanMeshShader( VulkanMeshShader&& ) = default;
		VulkanMeshShader& operator=( const VulkanMeshShader& ) = delete;
		VulkanMeshShader& operator=( VulkanMeshShader&& ) = default;
		virtual ~VulkanMeshShader() override = default;

	private:
		virtual void InitResource() override {}
		virtual void FreeResource() override {}
	};

	class VulkanAmplificationShader final : public AmplificationShader
	{
	public:
		VulkanAmplificationShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo )
			: AmplificationShader( byteCode, byteCodeSize, paramInfo ) {}
		VulkanAmplificationShader( const VulkanAmplificationShader& ) = delete;
		VulkanAmplificationShader( VulkanAmplificationShader&& ) = default;
		VulkanAmplificationShader& operator=( const VulkanAmplificationShader& ) = delete;
		VulkanAmplificationShader& operator=( VulkanAmplificationShader&& ) = default;
		virtual ~VulkanAmplificationShader() override = default;

	private:
		virtual void InitResource() override {}
		virtual void FreeResource() override {}
	};

	class VulkanRayGenerationShader final : public RayGenerationShader
	{
	public:
		VulkanRayGenerationShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName )
			: RayGenerationShader( byteCode, byteCodeSize, paramInfo, exportName ) {}
		VulkanRayGenerationShader( const VulkanRayGenerationShader& ) = delete;
		VulkanRayGenerationShader( VulkanRayGenerationShader&& ) = default;
		VulkanRayGenerationShader& operator=( const VulkanRayGenerationShader& ) = delete;
		VulkanRayGenerationShader& operator=( VulkanRayGenerationShader&& ) = default;
		virtual ~VulkanRayGenerationShader() override = default;

	private:
		virtual void InitResource() override {}
		virtual void FreeResource() override {}
	};

	class VulkanIntersectionShader final : public IntersectionShader
	{
	public:
		VulkanIntersectionShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName )
			: IntersectionShader( byteCode, byteCodeSize, paramInfo, exportName ) {}
		VulkanIntersectionShader( const VulkanIntersectionShader& ) = delete;
		VulkanIntersectionShader( VulkanIntersectionShader&& ) = default;
		VulkanIntersectionShader& operator=( const VulkanIntersectionShader& ) = delete;
		VulkanIntersectionShader& operator=( VulkanIntersectionShader&& ) = default;
		virtual ~VulkanIntersectionShader() override = default;

	private:
		virtual void InitResource() override {}
		virtual void FreeResource() override {}
	};

	class VulkanAnyHitShader final : public AnyHitShader
	{
	public:
		VulkanAnyHitShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName )
			: AnyHitShader( byteCode, byteCodeSize, paramInfo, exportName ) {}
		VulkanAnyHitShader( const VulkanAnyHitShader& ) = delete;
		VulkanAnyHitShader( VulkanAnyHitShader&& ) = default;
		VulkanAnyHitShader& operator=( const VulkanAnyHitShader& ) = delete;
		VulkanAnyHitShader& operator=( VulkanAnyHitShader&& ) = default;
		virtual ~VulkanAnyHitShader() override = default;

	private:
		virtual void InitResource() override {}
		virtual void FreeResource() override {}
	};

	class VulkanClosestHitShader final : public ClosestHitShader
	{
	public:
		VulkanClosestHitShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName )
			: ClosestHitShader( byteCode, byteCodeSize, paramInfo, exportName ) {
		}
		VulkanClosestHitShader( const VulkanClosestHitShader& ) = delete;
		VulkanClosestHitShader( VulkanClosestHitShader&& ) = default;
		VulkanClosestHitShader& operator=( const VulkanClosestHitShader& ) = delete;
		VulkanClosestHitShader& operator=( VulkanClosestHitShader&& ) = default;
		virtual ~VulkanClosestHitShader() override = default;

	private:
		virtual void InitResource() override {}
		virtual void FreeResource() override {}
	};

	class VulkanMissShader final : public MissShader
	{
	public:
		VulkanMissShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName )
			: MissShader( byteCode, byteCodeSize, paramInfo, exportName ) {
		}
		VulkanMissShader( const VulkanMissShader& ) = delete;
		VulkanMissShader( VulkanMissShader&& ) = default;
		VulkanMissShader& operator=( const VulkanMissShader& ) = delete;
		VulkanMissShader& operator=( VulkanMissShader&& ) = default;
		virtual ~VulkanMissShader() override = default;

	private:
		virtual void InitResource() override {}
		virtual void FreeResource() override {}
	};

	class VulkanCallableShader final : public CallableShader
	{
	public:
		VulkanCallableShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName )
			: CallableShader( byteCode, byteCodeSize, paramInfo, exportName ) {
		}
		VulkanCallableShader( const VulkanCallableShader& ) = delete;
		VulkanCallableShader( VulkanCallableShader&& ) = default;
		VulkanCallableShader& operator=( const VulkanCallableShader& ) = delete;
		VulkanCallableShader& operator=( VulkanCallableShader&& ) = default;
		virtual ~VulkanCallableShader() override = default;

	private:
		virtual void InitResource() override {}
		virtual void FreeResource() override {}
	};
}