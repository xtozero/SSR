#pragma once

#include "NameTypes.h"
#include "ShaderResource.h"
#include "SizedTypes.h"

#include <d3d11.h>
#include <d3dcompiler.h>

namespace agl
{
	AGL_DLL void ExtractShaderParameters( ID3D11ShaderReflection* pReflector, ShaderParameterMap& parameterMap );

	AGL_DLL void BuildShaderParameterInfo( const std::map<Name, ShaderParameter>& parameterMap, ShaderParameterInfo& parameterInfo );

	class D3D11VertexShader : public VertexShader, public ShaderBase
	{
	public:
		ID3D11VertexShader* Resource() { return m_pResource; }

		D3D11VertexShader( const void* byteCode, size_t byteCodeSize ) : ShaderBase( byteCode, byteCodeSize ) {}
		D3D11VertexShader( const D3D11VertexShader& ) = delete;
		D3D11VertexShader( D3D11VertexShader&& ) = default;
		D3D11VertexShader& operator=( const D3D11VertexShader& ) = delete;
		D3D11VertexShader& operator=( D3D11VertexShader&& ) = default;
		~D3D11VertexShader() = default;

	private:
		virtual void InitResource() override;
		virtual void FreeResource() override;

		ID3D11VertexShader* m_pResource = nullptr;
	};

	class D3D11GeometryShader : public GeometryShader, public ShaderBase
	{
	public:
		ID3D11GeometryShader* Resource() { return m_pResource; }

		D3D11GeometryShader( const void* byteCode, size_t byteCodeSize ) : ShaderBase( byteCode, byteCodeSize ) {}
		D3D11GeometryShader( const D3D11GeometryShader& ) = delete;
		D3D11GeometryShader( D3D11GeometryShader&& ) = default;
		D3D11GeometryShader& operator=( const D3D11GeometryShader& ) = delete;
		D3D11GeometryShader& operator=( D3D11GeometryShader&& ) = default;
		~D3D11GeometryShader() = default;

	private:
		virtual void InitResource() override;
		virtual void FreeResource() override;

		ID3D11GeometryShader* m_pResource = nullptr;
	};

	class D3D11PixelShader : public PixelShader, public ShaderBase
	{
	public:
		ID3D11PixelShader* Resource() { return m_pResource; }

		D3D11PixelShader( const void* byteCode, size_t byteCodeSize ) : ShaderBase( byteCode, byteCodeSize ) {}
		D3D11PixelShader( const D3D11PixelShader& ) = delete;
		D3D11PixelShader( D3D11PixelShader&& ) = default;
		D3D11PixelShader& operator=( const D3D11PixelShader& ) = delete;
		D3D11PixelShader& operator=( D3D11PixelShader&& ) = default;
		~D3D11PixelShader() = default;

	private:
		virtual void InitResource() override;
		virtual void FreeResource() override;

		ID3D11PixelShader* m_pResource = nullptr;
	};

	class D3D11ComputeShader : public ComputeShader, public ShaderBase
	{
	public:
		ID3D11ComputeShader* Resource() { return m_pResource; }

		D3D11ComputeShader( const void* byteCode, size_t byteCodeSize ) : ShaderBase( byteCode, byteCodeSize ) {}
		D3D11ComputeShader( const D3D11ComputeShader& ) = delete;
		D3D11ComputeShader( D3D11ComputeShader&& ) = default;
		D3D11ComputeShader& operator=( const D3D11ComputeShader& ) = delete;
		D3D11ComputeShader& operator=( D3D11ComputeShader&& ) = default;
		~D3D11ComputeShader() = default;

	private:
		virtual void InitResource() override;
		virtual void FreeResource() override;

		ID3D11ComputeShader* m_pResource = nullptr;
	};
}
