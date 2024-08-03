#pragma once

#include "common.h"
#include "GraphicsApiResource.h"
#include "ShaderParameterInfo.h"
#include "SizedTypes.h"

namespace agl
{
	class ShaderBase
	{
	public:
		const void* ByteCode() const { return m_byteCode; }
		size_t ByteCodeSize() const { return m_byteCodeSize; }

		const ShaderParameterInfo& GetParameterInfo() const
		{
			return *m_parameterInfo;
		}

		ShaderBase( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo )
			: m_byteCodeSize( byteCodeSize )
			, m_parameterInfo( &paramInfo )
		{
			m_byteCode = new unsigned char[m_byteCodeSize];
			std::memcpy( m_byteCode, byteCode, m_byteCodeSize );
		}

		virtual ~ShaderBase()
		{
			delete[] m_byteCode;
		}

		ShaderBase( const ShaderBase& ) = delete;
		ShaderBase( ShaderBase&& other ) = default;
		ShaderBase& operator=( const ShaderBase& ) = delete;
		ShaderBase& operator=( ShaderBase&& other ) = default;

	protected:
		void* m_byteCode = nullptr;
		size_t m_byteCodeSize = 0;
		const ShaderParameterInfo* m_parameterInfo = nullptr;
	};

	class VertexShader : public GraphicsApiResource
	{
	public:
		AGL_DLL static RefHandle<VertexShader> Create( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo );
	};

	class GeometryShader : public GraphicsApiResource
	{
	public:
		AGL_DLL static RefHandle<GeometryShader> Create( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo );
	};

	class PixelShader : public GraphicsApiResource
	{
	public:
		AGL_DLL static RefHandle<PixelShader> Create( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo );
	};

	class ComputeShader : public GraphicsApiResource
	{
	public:
		AGL_DLL static RefHandle<ComputeShader> Create( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo );
	};
}