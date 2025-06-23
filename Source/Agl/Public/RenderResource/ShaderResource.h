#pragma once

#include "GraphicsApiResource.h"
#include "LibraryTool/Common.h"
#include "ShaderParameterInfo.h"
#include "SizedTypes.h"

namespace agl
{
	class ShaderBase : public GraphicsApiResource
	{
	public:
		const void* ByteCode() const { return m_byteCode; }
		size_t ByteCodeSize() const { return m_byteCodeSize; }

		const ShaderParameterInfo& GetParameterInfo() const
		{
			return *m_parameterInfo;
		}

		AGL_DLL void UpdateByteCodeAndParameterInfo( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo );

		ShaderBase( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo )
			: m_byteCodeSize( byteCodeSize )
			, m_parameterInfo( &paramInfo )
		{
			m_byteCode = new uint8[m_byteCodeSize];
			std::memcpy( m_byteCode, byteCode, m_byteCodeSize );
		}

		virtual ~ShaderBase() override
		{
			delete[] m_byteCode;
		}

		ShaderBase( const ShaderBase& ) = delete;
		ShaderBase( ShaderBase&& other ) = default;
		ShaderBase& operator=( const ShaderBase& ) = delete;
		ShaderBase& operator=( ShaderBase&& other ) = default;

	protected:
		uint8* m_byteCode = nullptr;
		size_t m_byteCodeSize = 0;
		const ShaderParameterInfo* m_parameterInfo = nullptr;
	};

	class VertexShader : public ShaderBase
	{
	public:
		AGL_DLL static RefHandle<VertexShader> Create( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo );

		using ShaderBase::ShaderBase;
	};

	class GeometryShader : public ShaderBase
	{
	public:
		AGL_DLL static RefHandle<GeometryShader> Create( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo );

		using ShaderBase::ShaderBase;
	};

	class PixelShader : public ShaderBase
	{
	public:
		AGL_DLL static RefHandle<PixelShader> Create( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo );

		using ShaderBase::ShaderBase;
	};

	class ComputeShader : public ShaderBase
	{
	public:
		AGL_DLL static RefHandle<ComputeShader> Create( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo );

		using ShaderBase::ShaderBase;
	};

	class MeshShader : public ShaderBase
	{
	public:
		AGL_DLL static RefHandle<MeshShader> Create( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo );

		using ShaderBase::ShaderBase;
	};

	class AmplificationShader : public ShaderBase
	{
	public:
		AGL_DLL static RefHandle<AmplificationShader> Create( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo );

		using ShaderBase::ShaderBase;
	};
}