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

	bool IsRayTracingShader( ShaderType shaderType )
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
		return GetInterface<IResourceManager>( )->CreateVertexShader( byteCode, byteCodeSize, paramInfo );
	}

	RefHandle<GeometryShader> GeometryShader::Create( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo )
	{
		return GetInterface<IResourceManager>( )->CreateGeometryShader( byteCode, byteCodeSize, paramInfo );
	}

	RefHandle<PixelShader> PixelShader::Create( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo )
	{
		return GetInterface<IResourceManager>( )->CreatePixelShader( byteCode, byteCodeSize, paramInfo );
	}

	RefHandle<ComputeShader> ComputeShader::Create( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo )
	{
		return GetInterface<IResourceManager>( )->CreateComputeShader( byteCode, byteCodeSize, paramInfo );
	}

	RefHandle<MeshShader> MeshShader::Create( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo )
	{
		return GetInterface<IResourceManager>()->CreateMeshShader( byteCode, byteCodeSize, paramInfo );
	}

	RefHandle<AmplificationShader> AmplificationShader::Create( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo )
	{
		return GetInterface<IResourceManager>()->CreateAmplificationShader( byteCode, byteCodeSize, paramInfo );
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
		return GetInterface<IResourceManager>()->CreateRayGenerationShader( byteCode, byteCodeSize, paramInfo, exportName );
	}

	RefHandle<IntersectionShader> IntersectionShader::Create( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName )
	{
		return GetInterface<IResourceManager>()->CreateIntersectionShader( byteCode, byteCodeSize, paramInfo, exportName );
	}

	RefHandle<AnyHitShader> AnyHitShader::Create( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName )
	{
		return GetInterface<IResourceManager>()->CreateAnyHitShader( byteCode, byteCodeSize, paramInfo, exportName );
	}

	RefHandle<ClosestHitShader> ClosestHitShader::Create( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName )
	{
		return GetInterface<IResourceManager>()->CreateClosestHitShader( byteCode, byteCodeSize, paramInfo, exportName );
	}

	RefHandle<MissShader> MissShader::Create( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName )
	{
		return GetInterface<IResourceManager>()->CreateMissShader( byteCode, byteCodeSize, paramInfo, exportName );
	}

	RefHandle<CallableShader> CallableShader::Create( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName )
	{
		return GetInterface<IResourceManager>()->CreateCallableShader( byteCode, byteCodeSize, paramInfo, exportName );
	}
}
