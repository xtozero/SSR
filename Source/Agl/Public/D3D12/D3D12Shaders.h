#pragma once

#include "ShaderResource.h"

#include <d3d12.h>

namespace agl
{
	class D3D12VertexShader final : public VertexShader
	{
	public:
		D3D12VertexShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo )
			: VertexShader( byteCode, byteCodeSize, paramInfo ) {}
		D3D12VertexShader( const D3D12VertexShader& ) = delete;
		D3D12VertexShader( D3D12VertexShader&& ) = default;
		D3D12VertexShader& operator=( const D3D12VertexShader& ) = delete;
		D3D12VertexShader& operator=( D3D12VertexShader&& ) = default;
		virtual ~D3D12VertexShader() override = default;

	private:
		virtual void InitResource() override {}
		virtual void FreeResource() override {}
	};

	class D3D12GeometryShader final : public GeometryShader
	{
	public:
		D3D12GeometryShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo )
			: GeometryShader( byteCode, byteCodeSize, paramInfo ) {}
		D3D12GeometryShader( const D3D12GeometryShader& ) = delete;
		D3D12GeometryShader( D3D12GeometryShader&& ) = default;
		D3D12GeometryShader& operator=( const D3D12GeometryShader& ) = delete;
		D3D12GeometryShader& operator=( D3D12GeometryShader&& ) = default;
		virtual ~D3D12GeometryShader() override = default;

	private:
		virtual void InitResource() override {}
		virtual void FreeResource() override {}
	};

	class D3D12PixelShader final : public PixelShader
	{
	public:
		D3D12PixelShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo )
			: PixelShader( byteCode, byteCodeSize, paramInfo ) {}
		D3D12PixelShader( const D3D12PixelShader& ) = delete;
		D3D12PixelShader( D3D12PixelShader&& ) = default;
		D3D12PixelShader& operator=( const D3D12PixelShader& ) = delete;
		D3D12PixelShader& operator=( D3D12PixelShader&& ) = default;
		virtual ~D3D12PixelShader() override = default;

	private:
		virtual void InitResource() override {}
		virtual void FreeResource() override {}
	};

	class D3D12ComputeShader final : public ComputeShader
	{
	public:
		D3D12ComputeShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo )
			: ComputeShader( byteCode, byteCodeSize, paramInfo ) {}
		D3D12ComputeShader( const D3D12ComputeShader& ) = delete;
		D3D12ComputeShader( D3D12ComputeShader&& ) = default;
		D3D12ComputeShader& operator=( const D3D12ComputeShader& ) = delete;
		D3D12ComputeShader& operator=( D3D12ComputeShader&& ) = default;
		virtual ~D3D12ComputeShader() override = default;

	private:
		virtual void InitResource() override {}
		virtual void FreeResource() override {}
	};

	class D3D12MeshShader final : public MeshShader
	{
	public:
		D3D12MeshShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo )
			: MeshShader( byteCode, byteCodeSize, paramInfo ) {}
		D3D12MeshShader( const D3D12MeshShader& ) = delete;
		D3D12MeshShader( D3D12MeshShader&& ) = default;
		D3D12MeshShader& operator=( const D3D12MeshShader& ) = delete;
		D3D12MeshShader& operator=( D3D12MeshShader&& ) = default;
		virtual ~D3D12MeshShader() override = default;

	private:
		virtual void InitResource() override {}
		virtual void FreeResource() override {}
	};

	class D3D12AmplificationShader final : public AmplificationShader
	{
	public:
		D3D12AmplificationShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo )
			: AmplificationShader( byteCode, byteCodeSize, paramInfo ) {}
		D3D12AmplificationShader( const D3D12AmplificationShader& ) = delete;
		D3D12AmplificationShader( D3D12AmplificationShader&& ) = default;
		D3D12AmplificationShader& operator=( const D3D12AmplificationShader& ) = delete;
		D3D12AmplificationShader& operator=( D3D12AmplificationShader&& ) = default;
		virtual ~D3D12AmplificationShader() override = default;

	private:
		virtual void InitResource() override {}
		virtual void FreeResource() override {}
	};

	class D3D12RayGenerationShader final : public RayGenerationShader
	{
	public:
		D3D12RayGenerationShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName )
			: RayGenerationShader( byteCode, byteCodeSize, paramInfo, exportName ) {}
		D3D12RayGenerationShader( const D3D12RayGenerationShader& ) = delete;
		D3D12RayGenerationShader( D3D12RayGenerationShader&& ) = default;
		D3D12RayGenerationShader& operator=( const D3D12RayGenerationShader& ) = delete;
		D3D12RayGenerationShader& operator=( D3D12RayGenerationShader&& ) = default;
		virtual ~D3D12RayGenerationShader() override = default;

	private:
		virtual void InitResource() override {}
		virtual void FreeResource() override {}
	};

	class D3D12IntersectionShader final : public IntersectionShader
	{
	public:
		D3D12IntersectionShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName )
			: IntersectionShader( byteCode, byteCodeSize, paramInfo, exportName ) {}
		D3D12IntersectionShader( const D3D12IntersectionShader& ) = delete;
		D3D12IntersectionShader( D3D12IntersectionShader&& ) = default;
		D3D12IntersectionShader& operator=( const D3D12IntersectionShader& ) = delete;
		D3D12IntersectionShader& operator=( D3D12IntersectionShader&& ) = default;
		virtual ~D3D12IntersectionShader() override = default;

	private:
		virtual void InitResource() override {}
		virtual void FreeResource() override {}
	};

	class D3D12AnyHitShader final : public AnyHitShader
	{
	public:
		D3D12AnyHitShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName )
			: AnyHitShader( byteCode, byteCodeSize, paramInfo, exportName ) {}
		D3D12AnyHitShader( const D3D12AnyHitShader& ) = delete;
		D3D12AnyHitShader( D3D12AnyHitShader&& ) = default;
		D3D12AnyHitShader& operator=( const D3D12AnyHitShader& ) = delete;
		D3D12AnyHitShader& operator=( D3D12AnyHitShader&& ) = default;
		virtual ~D3D12AnyHitShader() override = default;

	private:
		virtual void InitResource() override {}
		virtual void FreeResource() override {}
	};

	class D3D12ClosestHitShader final : public ClosestHitShader
	{
	public:
		D3D12ClosestHitShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName )
			: ClosestHitShader( byteCode, byteCodeSize, paramInfo, exportName ) {
		}
		D3D12ClosestHitShader( const D3D12ClosestHitShader& ) = delete;
		D3D12ClosestHitShader( D3D12ClosestHitShader&& ) = default;
		D3D12ClosestHitShader& operator=( const D3D12ClosestHitShader& ) = delete;
		D3D12ClosestHitShader& operator=( D3D12ClosestHitShader&& ) = default;
		virtual ~D3D12ClosestHitShader() override = default;

	private:
		virtual void InitResource() override {}
		virtual void FreeResource() override {}
	};

	class D3D12MissShader final : public MissShader
	{
	public:
		D3D12MissShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName )
			: MissShader( byteCode, byteCodeSize, paramInfo, exportName ) {
		}
		D3D12MissShader( const D3D12MissShader& ) = delete;
		D3D12MissShader( D3D12MissShader&& ) = default;
		D3D12MissShader& operator=( const D3D12MissShader& ) = delete;
		D3D12MissShader& operator=( D3D12MissShader&& ) = default;
		virtual ~D3D12MissShader() override = default;

	private:
		virtual void InitResource() override {}
		virtual void FreeResource() override {}
	};

	class D3D12CallableShader final : public CallableShader
	{
	public:
		D3D12CallableShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName )
			: CallableShader( byteCode, byteCodeSize, paramInfo, exportName ) {
		}
		D3D12CallableShader( const D3D12CallableShader& ) = delete;
		D3D12CallableShader( D3D12CallableShader&& ) = default;
		D3D12CallableShader& operator=( const D3D12CallableShader& ) = delete;
		D3D12CallableShader& operator=( D3D12CallableShader&& ) = default;
		virtual ~D3D12CallableShader() override = default;

	private:
		virtual void InitResource() override {}
		virtual void FreeResource() override {}
	};
}