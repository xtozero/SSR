#include "ShaderResource.h"

#include "InterfaceFactories.h"
#include "IResourceManager.h"

namespace agl
{
	const char* ToString( ShaderType shaderType )
	{
		const char* shaderTypeStr[] = {
			"VS",
			"HS",
			"DS",
			"GS",
			"PS",
			"CS",
			"MS",
			"AS",
			"RayGen",
			"Intersection",
			"AnyHit",
			"ClosestHit",
			"Miss",
			"Callable",
		};

		return shaderTypeStr[static_cast<uint32>( shaderType )];
	}

	bool IsRaytracingShader( ShaderType shaderType )
	{
		return shaderType == ShaderType::RayGen
			|| shaderType == ShaderType::Intersection
			|| shaderType == ShaderType::AnyHit
			|| shaderType == ShaderType::ClosestHit
			|| shaderType == ShaderType::Miss
			|| shaderType == ShaderType::Callable;
	}

	void Shader::UpdateByteCodeAndParameterInfo( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo )
	{
		m_byteCodeSize = byteCodeSize;

		delete[] m_byteCode;
		m_byteCode = new uint8[m_byteCodeSize];
		std::memcpy( m_byteCode, byteCode, m_byteCodeSize );

		m_parameterInfo = &paramInfo;
	}

	RefHandle<VertexShader> VertexShader::Create( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo )
	{
		auto newShader = GetInterface<IResourceManager>( )->CreateVertexShader( byteCode, byteCodeSize, paramInfo );
		EnqueueRenderTask(
			[shader = newShader]
			{
				shader->Init();
			});

		return newShader;
	}

	RefHandle<GeometryShader> GeometryShader::Create( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo )
	{
		auto newShader = GetInterface<IResourceManager>( )->CreateGeometryShader( byteCode, byteCodeSize, paramInfo );
		EnqueueRenderTask(
			[shader = newShader]
			{
				shader->Init();
			});

		return newShader;
	}

	RefHandle<PixelShader> PixelShader::Create( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo )
	{
		auto newShader = GetInterface<IResourceManager>( )->CreatePixelShader( byteCode, byteCodeSize, paramInfo );
		EnqueueRenderTask(
			[shader = newShader]
			{
				shader->Init();
			});

		return newShader;
	}

	RefHandle<ComputeShader> ComputeShader::Create( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo )
	{
		auto newShader = GetInterface<IResourceManager>( )->CreateComputeShader( byteCode, byteCodeSize, paramInfo );
		EnqueueRenderTask(
			[shader = newShader]
			{
				shader->Init();
			});

		return newShader;
	}

	RefHandle<MeshShader> MeshShader::Create( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo )
	{
		auto newShader = GetInterface<IResourceManager>()->CreateMeshShader( byteCode, byteCodeSize, paramInfo );
		EnqueueRenderTask(
			[shader = newShader]
			{
				shader->Init();
			});

		return newShader;
	}

	RefHandle<AmplificationShader> AmplificationShader::Create( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo )
	{
		auto newShader = GetInterface<IResourceManager>()->CreateAmplificationShader( byteCode, byteCodeSize, paramInfo );
		EnqueueRenderTask(
			[shader = newShader]
			{
				shader->Init();
			});

		return newShader;
	}

	const wchar_t* RaytracingShader::GetExportName() const
	{
		return m_wExportName;
	}

	RaytracingShader::RaytracingShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName )
		: Shader( byteCode, byteCodeSize, paramInfo )
	{
		ToWideChar( m_wExportName, ExportNameBufferSize, exportName.CStr() );
	}

	RefHandle<RayGenerationShader> RayGenerationShader::Create( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName )
	{
		auto newShader = GetInterface<IResourceManager>()->CreateRayGenerationShader( byteCode, byteCodeSize, paramInfo, exportName );
		EnqueueRenderTask(
			[shader = newShader]
			{
				shader->Init();
			});

		return newShader;
	}

	RefHandle<IntersectionShader> IntersectionShader::Create( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName )
	{
		auto newShader = GetInterface<IResourceManager>()->CreateIntersectionShader( byteCode, byteCodeSize, paramInfo, exportName );
		EnqueueRenderTask(
			[shader = newShader]
			{
				shader->Init();
			});

		return newShader;
	}

	RefHandle<AnyHitShader> AnyHitShader::Create( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName )
	{
		auto newShader = GetInterface<IResourceManager>()->CreateAnyHitShader( byteCode, byteCodeSize, paramInfo, exportName );
		EnqueueRenderTask(
			[shader = newShader]
			{
				shader->Init();
			});

		return newShader;
	}

	RefHandle<ClosestHitShader> ClosestHitShader::Create( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName )
	{
		auto newShader = GetInterface<IResourceManager>()->CreateClosestHitShader( byteCode, byteCodeSize, paramInfo, exportName );
		EnqueueRenderTask(
			[shader = newShader]
			{
				shader->Init();
			});

		return newShader;
	}

	RefHandle<MissShader> MissShader::Create( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName )
	{
		auto newShader = GetInterface<IResourceManager>()->CreateMissShader( byteCode, byteCodeSize, paramInfo, exportName );
		EnqueueRenderTask(
			[shader = newShader]
			{
				shader->Init();
			});

		return newShader;
	}

	RefHandle<CallableShader> CallableShader::Create( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName )
	{
		auto newShader = GetInterface<IResourceManager>()->CreateCallableShader( byteCode, byteCodeSize, paramInfo, exportName );
		EnqueueRenderTask(
			[shader = newShader]
			{
				shader->Init();
			});

		return newShader;
	}
}
