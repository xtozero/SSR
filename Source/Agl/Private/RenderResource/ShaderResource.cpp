#include "ShaderResource.h"

#include "InterfaceFactories.h"
#include "IRenderResourceManager.h"

namespace agl
{
	void Shader::UpdateByteCodeAndParameterInfo( const void* byteCode, size_t byteCodeSize,
		const ShaderParameterInfo& paramInfo )
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
}
