#pragma once

#include "VulkanApi.h"

namespace agl
{
	constexpr VkShaderStageFlagBits ConvertToShaderStage( ShaderType shaderType )
	{
		switch ( shaderType )
		{
		case ShaderType::Vertex:
			return VK_SHADER_STAGE_VERTEX_BIT;
		case ShaderType::Geometry:
			return VK_SHADER_STAGE_GEOMETRY_BIT;
		case ShaderType::Pixel:
			return VK_SHADER_STAGE_FRAGMENT_BIT;
		case ShaderType::Compute:
			return VK_SHADER_STAGE_COMPUTE_BIT;
		case ShaderType::Amplification:
			return VK_SHADER_STAGE_TASK_BIT_EXT;
		case ShaderType::Mesh:
			return VK_SHADER_STAGE_MESH_BIT_EXT;
		case ShaderType::RayGen:
			return VK_SHADER_STAGE_RAYGEN_BIT_KHR;
		case ShaderType::Intersection:
			return VK_SHADER_STAGE_INTERSECTION_BIT_KHR;
		case ShaderType::AnyHit:
			return VK_SHADER_STAGE_ANY_HIT_BIT_KHR;
		case ShaderType::ClosestHit:
			return VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
		case ShaderType::Miss:
			return VK_SHADER_STAGE_MISS_BIT_KHR;
		case ShaderType::Callable:
			return VK_SHADER_STAGE_CALLABLE_BIT_KHR;
		}

		return VK_SHADER_STAGE_ALL;
	}

	template <ShaderType shaderType, typename BaseClass>
	class VulkanShaderBase : public BaseClass
	{
	public:
		VkPipelineShaderStageCreateInfo GetShaderStageCreateInfo() const
		{
			VkPipelineShaderStageCreateInfo info = {
				.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
				.stage = ConvertToShaderStage( shaderType ),
				.module = m_shaderModule,
				// .pName =
			};

			return info;
		}

	protected:
		VulkanShaderBase( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo )
			: BaseClass( byteCode, byteCodeSize, paramInfo ) {}

		VulkanShaderBase( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName )
			: BaseClass( byteCode, byteCodeSize, paramInfo, exportName ) {}

		VkShaderModule m_shaderModule = VK_NULL_HANDLE;

	private:
		virtual void InitResource() override
		{
			VkShaderModuleCreateInfo createInfo = {
				.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
				.codeSize = BaseClass::ByteCodeSize(),
				.pCode = static_cast<const uint32*>( BaseClass::ByteCode() ),
			};

			VkResult result = vkCreateShaderModule( VulkanDevice(), &createInfo, nullptr, &m_shaderModule );
			assert( result == VK_SUCCESS );
		}

		virtual void FreeResource() override
		{
			if ( m_shaderModule != VK_NULL_HANDLE )
			{
				vkDestroyShaderModule( VulkanDevice(), m_shaderModule, nullptr );
			}
		}
	};

	class VulkanVertexShader final : public VulkanShaderBase<ShaderType::Vertex, VertexShader>
	{
	public:
		VulkanVertexShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo )
			: VulkanShaderBase( byteCode, byteCodeSize, paramInfo ) {}
		VulkanVertexShader( const VulkanVertexShader& ) = delete;
		VulkanVertexShader( VulkanVertexShader&& ) = default;
		VulkanVertexShader& operator=( const VulkanVertexShader& ) = delete;
		VulkanVertexShader& operator=( VulkanVertexShader&& ) = default;
		virtual ~VulkanVertexShader() override = default;
	};

	class VulkanGeometryShader final : public VulkanShaderBase<ShaderType::Geometry, GeometryShader>
	{
	public:
		VulkanGeometryShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo )
			: VulkanShaderBase( byteCode, byteCodeSize, paramInfo ) {}
		VulkanGeometryShader( const VulkanGeometryShader& ) = delete;
		VulkanGeometryShader( VulkanGeometryShader&& ) = default;
		VulkanGeometryShader& operator=( const VulkanGeometryShader& ) = delete;
		VulkanGeometryShader& operator=( VulkanGeometryShader&& ) = default;
		virtual ~VulkanGeometryShader() override = default;
	};

	class VulkanPixelShader final : public VulkanShaderBase<ShaderType::Pixel, PixelShader>
	{
	public:
		VulkanPixelShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo )
			: VulkanShaderBase( byteCode, byteCodeSize, paramInfo ) {}
		VulkanPixelShader( const VulkanPixelShader& ) = delete;
		VulkanPixelShader( VulkanPixelShader&& ) = default;
		VulkanPixelShader& operator=( const VulkanPixelShader& ) = delete;
		VulkanPixelShader& operator=( VulkanPixelShader&& ) = default;
		virtual ~VulkanPixelShader() override = default;
	};

	class VulkanComputeShader final : public VulkanShaderBase<ShaderType::Compute, ComputeShader>
	{
	public:
		VulkanComputeShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo )
			: VulkanShaderBase( byteCode, byteCodeSize, paramInfo ) {}
		VulkanComputeShader( const VulkanComputeShader& ) = delete;
		VulkanComputeShader( VulkanComputeShader&& ) = default;
		VulkanComputeShader& operator=( const VulkanComputeShader& ) = delete;
		VulkanComputeShader& operator=( VulkanComputeShader&& ) = default;
		virtual ~VulkanComputeShader() override = default;
	};

	class VulkanMeshShader final : public VulkanShaderBase<ShaderType::Mesh, MeshShader>
	{
	public:
		VulkanMeshShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo )
			: VulkanShaderBase( byteCode, byteCodeSize, paramInfo ) {}
		VulkanMeshShader( const VulkanMeshShader& ) = delete;
		VulkanMeshShader( VulkanMeshShader&& ) = default;
		VulkanMeshShader& operator=( const VulkanMeshShader& ) = delete;
		VulkanMeshShader& operator=( VulkanMeshShader&& ) = default;
		virtual ~VulkanMeshShader() override = default;
	};

	class VulkanAmplificationShader final : public VulkanShaderBase<ShaderType::Amplification, AmplificationShader>
	{
	public:
		VulkanAmplificationShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo )
			: VulkanShaderBase( byteCode, byteCodeSize, paramInfo ) {}
		VulkanAmplificationShader( const VulkanAmplificationShader& ) = delete;
		VulkanAmplificationShader( VulkanAmplificationShader&& ) = default;
		VulkanAmplificationShader& operator=( const VulkanAmplificationShader& ) = delete;
		VulkanAmplificationShader& operator=( VulkanAmplificationShader&& ) = default;
		virtual ~VulkanAmplificationShader() override = default;
	};

	class VulkanRayGenerationShader final : public VulkanShaderBase<ShaderType::RayGen, RayGenerationShader>
	{
	public:
		VulkanRayGenerationShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName )
			: VulkanShaderBase( byteCode, byteCodeSize, paramInfo, exportName ) {}
		VulkanRayGenerationShader( const VulkanRayGenerationShader& ) = delete;
		VulkanRayGenerationShader( VulkanRayGenerationShader&& ) = default;
		VulkanRayGenerationShader& operator=( const VulkanRayGenerationShader& ) = delete;
		VulkanRayGenerationShader& operator=( VulkanRayGenerationShader&& ) = default;
		virtual ~VulkanRayGenerationShader() override = default;

	private:
		virtual void InitResource() override {}
		virtual void FreeResource() override {}
	};

	class VulkanIntersectionShader final : public VulkanShaderBase<ShaderType::Intersection, IntersectionShader>
	{
	public:
		VulkanIntersectionShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName )
			: VulkanShaderBase( byteCode, byteCodeSize, paramInfo, exportName ) {}
		VulkanIntersectionShader( const VulkanIntersectionShader& ) = delete;
		VulkanIntersectionShader( VulkanIntersectionShader&& ) = default;
		VulkanIntersectionShader& operator=( const VulkanIntersectionShader& ) = delete;
		VulkanIntersectionShader& operator=( VulkanIntersectionShader&& ) = default;
		virtual ~VulkanIntersectionShader() override = default;

	private:
		virtual void InitResource() override {}
		virtual void FreeResource() override {}
	};

	class VulkanAnyHitShader final : public VulkanShaderBase<ShaderType::AnyHit, AnyHitShader>
	{
	public:
		VulkanAnyHitShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName )
			: VulkanShaderBase( byteCode, byteCodeSize, paramInfo, exportName ) {}
		VulkanAnyHitShader( const VulkanAnyHitShader& ) = delete;
		VulkanAnyHitShader( VulkanAnyHitShader&& ) = default;
		VulkanAnyHitShader& operator=( const VulkanAnyHitShader& ) = delete;
		VulkanAnyHitShader& operator=( VulkanAnyHitShader&& ) = default;
		virtual ~VulkanAnyHitShader() override = default;

	private:
		virtual void InitResource() override {}
		virtual void FreeResource() override {}
	};

	class VulkanClosestHitShader final : public VulkanShaderBase<ShaderType::ClosestHit, ClosestHitShader>
	{
	public:
		VulkanClosestHitShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName )
			: VulkanShaderBase( byteCode, byteCodeSize, paramInfo, exportName ) {}
		VulkanClosestHitShader( const VulkanClosestHitShader& ) = delete;
		VulkanClosestHitShader( VulkanClosestHitShader&& ) = default;
		VulkanClosestHitShader& operator=( const VulkanClosestHitShader& ) = delete;
		VulkanClosestHitShader& operator=( VulkanClosestHitShader&& ) = default;
		virtual ~VulkanClosestHitShader() override = default;

	private:
		virtual void InitResource() override {}
		virtual void FreeResource() override {}
	};

	class VulkanMissShader final : public VulkanShaderBase<ShaderType::Miss, MissShader>
	{
	public:
		VulkanMissShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName )
			: VulkanShaderBase( byteCode, byteCodeSize, paramInfo, exportName ) {}
		VulkanMissShader( const VulkanMissShader& ) = delete;
		VulkanMissShader( VulkanMissShader&& ) = default;
		VulkanMissShader& operator=( const VulkanMissShader& ) = delete;
		VulkanMissShader& operator=( VulkanMissShader&& ) = default;
		virtual ~VulkanMissShader() override = default;

	private:
		virtual void InitResource() override {}
		virtual void FreeResource() override {}
	};

	class VulkanCallableShader final : public VulkanShaderBase<ShaderType::Callable, CallableShader>
	{
	public:
		VulkanCallableShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName )
			: VulkanShaderBase( byteCode, byteCodeSize, paramInfo, exportName ) {}
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