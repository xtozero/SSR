#pragma once

#include "GraphicsApiResource.h"
#include "LibraryTool/Common.h"
#include "RefHandle.h"
#include "SizedTypes.h"

namespace agl
{
	class ShaderParameterInfo;

	enum class ShaderType : int8
	{
		Vertex,
		Hull,
		Domain,
		Geometry,
		Pixel,
		Compute,
		Mesh,
		Amplification,

		RayGen,
		Intersection,
		AnyHit,
		ClosestHit,
		Miss,
		Callable,

		Count,
		RayTracing = Compute,
		GraphicsCount = 8,
		None = -1,
	};

	template <typename T> requires std::is_integral_v<T>
	constexpr T NumShaderTypes = static_cast<T>( ShaderType::Count );

	template <typename T> requires std::is_integral_v<T>
	constexpr T NumGraphicsShaderTypes = static_cast<T>( ShaderType::GraphicsCount );

	AGL_FUNC_DLL const char* ToString( ShaderType shaderType );

	bool IsRayTracingShader( ShaderType shaderType );

	class Shader : public GraphicsApiResource
	{
	public:
		const void* ByteCode() const { return m_byteCode; }
		size_t ByteCodeSize() const { return m_byteCodeSize; }

		const ShaderParameterInfo& GetParameterInfo() const
		{
			return *m_parameterInfo;
		}

		AGL_DLL void UpdateByteCodeAndParameterInfo( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo );

		Shader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo )
			: m_byteCodeSize( byteCodeSize )
			, m_parameterInfo( &paramInfo )
		{
			m_byteCode = new uint8[m_byteCodeSize];
			std::memcpy( m_byteCode, byteCode, m_byteCodeSize );
		}

		virtual ~Shader() override
		{
			delete[] m_byteCode;
		}

		Shader( const Shader& ) = delete;
		Shader( Shader&& other ) = default;
		Shader& operator=( const Shader& ) = delete;
		Shader& operator=( Shader&& other ) = default;

	protected:
		uint8* m_byteCode = nullptr;
		size_t m_byteCodeSize = 0;
		const ShaderParameterInfo* m_parameterInfo = nullptr;
	};

	class VertexShader : public Shader
	{
	public:
		AGL_DLL static RefHandle<VertexShader> Create( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo );

		using Shader::Shader;
	};

	class GeometryShader : public Shader
	{
	public:
		AGL_DLL static RefHandle<GeometryShader> Create( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo );

		using Shader::Shader;
	};

	class PixelShader : public Shader
	{
	public:
		AGL_DLL static RefHandle<PixelShader> Create( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo );

		using Shader::Shader;
	};

	class ComputeShader : public Shader
	{
	public:
		AGL_DLL static RefHandle<ComputeShader> Create( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo );

		using Shader::Shader;
	};

	class MeshShader : public Shader
	{
	public:
		AGL_DLL static RefHandle<MeshShader> Create( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo );

		using Shader::Shader;
	};

	class AmplificationShader : public Shader
	{
	public:
		AGL_DLL static RefHandle<AmplificationShader> Create( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo );

		using Shader::Shader;
	};

	class RaytracingShader : public Shader
	{
	public:
		const wchar_t* GetExportName() const;

		RaytracingShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName );

		static constexpr int32 ExportNameBufferSize = 32;

	protected:
		wchar_t m_wExportName[ExportNameBufferSize] = {};
	};

	class RayGenerationShader : public RaytracingShader
	{
	public:
		AGL_DLL static RefHandle<RayGenerationShader> Create( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName );

		using RaytracingShader::RaytracingShader;
	};

	class IntersectionShader : public RaytracingShader
	{
	public:
		AGL_DLL static RefHandle<IntersectionShader> Create( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName );

		using RaytracingShader::RaytracingShader;
	};

	class AnyHitShader : public RaytracingShader
	{
	public:
		AGL_DLL static RefHandle<AnyHitShader> Create( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName );

		using RaytracingShader::RaytracingShader;
	};

	class ClosestHitShader : public RaytracingShader
	{
	public:
		AGL_DLL static RefHandle<ClosestHitShader> Create( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName );

		using RaytracingShader::RaytracingShader;
	};

	class MissShader : public RaytracingShader
	{
	public:
		AGL_DLL static RefHandle<MissShader> Create( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName );

		using RaytracingShader::RaytracingShader;
	};

	class CallableShader : public RaytracingShader
	{
	public:
		AGL_DLL static RefHandle<CallableShader> Create( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName );

		using RaytracingShader::RaytracingShader;
	};

	// bIdx: bindless index
	constexpr std::string_view BindlessIndexTag( "bidx_" );
}