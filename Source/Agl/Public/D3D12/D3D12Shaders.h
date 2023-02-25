#pragma once

#include "ShaderResource.h"

#include <d3d12.h>

namespace agl
{
	class D3D12VertexShader : public VertexShader, public ShaderBase
	{
	public:
		D3D12VertexShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo )
			: ShaderBase( byteCode, byteCodeSize, paramInfo ) {}
		D3D12VertexShader( const D3D12VertexShader& ) = delete;
		D3D12VertexShader( D3D12VertexShader&& ) = default;
		D3D12VertexShader& operator=( const D3D12VertexShader& ) = delete;
		D3D12VertexShader& operator=( D3D12VertexShader&& ) = default;
		virtual ~D3D12VertexShader() override = default;

	private:
		virtual void InitResource() override {}
		virtual void FreeResource() override {}
	};

	class D3D12GeometryShader : public GeometryShader, public ShaderBase
	{
	public:
		D3D12GeometryShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo )
			: ShaderBase( byteCode, byteCodeSize, paramInfo ) {}
		D3D12GeometryShader( const D3D12GeometryShader& ) = delete;
		D3D12GeometryShader( D3D12GeometryShader&& ) = default;
		D3D12GeometryShader& operator=( const D3D12GeometryShader& ) = delete;
		D3D12GeometryShader& operator=( D3D12GeometryShader&& ) = default;
		virtual ~D3D12GeometryShader() override = default;

	private:
		virtual void InitResource() override {}
		virtual void FreeResource() override {}
	};

	class D3D12PixelShader : public PixelShader, public ShaderBase
	{
	public:
		D3D12PixelShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo )
			: ShaderBase( byteCode, byteCodeSize, paramInfo ) {}
		D3D12PixelShader( const D3D12PixelShader& ) = delete;
		D3D12PixelShader( D3D12PixelShader&& ) = default;
		D3D12PixelShader& operator=( const D3D12PixelShader& ) = delete;
		D3D12PixelShader& operator=( D3D12PixelShader&& ) = default;
		virtual ~D3D12PixelShader() override = default;

	private:
		virtual void InitResource() override {}
		virtual void FreeResource() override {}
	};

	class D3D12ComputeShader : public ComputeShader, public ShaderBase
	{
	public:
		D3D12ComputeShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo )
			: ShaderBase( byteCode, byteCodeSize, paramInfo ) {}
		D3D12ComputeShader( const D3D12ComputeShader& ) = delete;
		D3D12ComputeShader( D3D12ComputeShader&& ) = default;
		D3D12ComputeShader& operator=( const D3D12ComputeShader& ) = delete;
		D3D12ComputeShader& operator=( D3D12ComputeShader&& ) = default;
		virtual ~D3D12ComputeShader() override = default;

	private:
		virtual void InitResource() override {}
		virtual void FreeResource() override {}
	};
}