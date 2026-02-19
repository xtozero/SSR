#include "ShaderResource.h"

#include "InterfaceFactories.h"
#include "IRenderResourceManager.h"

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

	RefHandle<MeshShader> agl::MeshShader::Create( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo )
	{
		return GetInterface<IResourceManager>()->CreateMeshShader( byteCode, byteCodeSize, paramInfo );
	}

	RefHandle<AmplificationShader> agl::AmplificationShader::Create( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo )
	{
		return GetInterface<IResourceManager>()->CreateAmplificationShader( byteCode, byteCodeSize, paramInfo );
	}

	RefHandle<RayGenerationShader> RayGenerationShader::Create( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo )
	{
		return GetInterface<IResourceManager>()->CreateRayGenerationShader( byteCode, byteCodeSize, paramInfo );
	}

	RefHandle<IntersectionShader> IntersectionShader::Create( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo )
	{
		return GetInterface<IResourceManager>()->CreateIntersectionShader( byteCode, byteCodeSize, paramInfo );
	}

	RefHandle<AnyHitShader> AnyHitShader::Create( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo )
	{
		return GetInterface<IResourceManager>()->CreateAnyHitShader( byteCode, byteCodeSize, paramInfo );
	}

	RefHandle<ClosestHitShader> ClosestHitShader::Create( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo )
	{
		return GetInterface<IResourceManager>()->CreateClosestHitShader( byteCode, byteCodeSize, paramInfo );
	}

	RefHandle<MissShader> MissShader::Create( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo )
	{
		return GetInterface<IResourceManager>()->CreateMissShader( byteCode, byteCodeSize, paramInfo );
	}

	RefHandle<CallableShader> CallableShader::Create( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo )
	{
		return GetInterface<IResourceManager>()->CreateCallableShader( byteCode, byteCodeSize, paramInfo );
	}
}
