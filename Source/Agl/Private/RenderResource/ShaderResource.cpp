#include "ShaderResource.h"

#include "InterfaceFactories.h"
#include "IRenderResourceManager.h"

namespace agl
{
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

	RefHandle<AmplificationShader> agl::AmplificationShader::Create( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo )
	{
		return GetInterface<IResourceManager>()->CreateAmplificationShader( byteCode, byteCodeSize, paramInfo );
	}

	RefHandle<MeshShader> agl::MeshShader::Create( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo )
	{
		return GetInterface<IResourceManager>()->CreateMeshShader( byteCode, byteCodeSize, paramInfo );
	}
}
