#pragma once

#include "NameTypes.h"
#include "ShaderResource.h"
#include "ShaderParameterInfo.h"
#include "SizedTypes.h"

#include <d3d11.h>
#include <d3dcompiler.h>

AGA_DLL void ExtractShaderParameters( ID3D11ShaderReflection* pReflector, aga::ShaderParameterMap& parameterMap );

AGA_DLL void BuildShaderParameterInfo( const std::map<Name, aga::ShaderParameter>& parameterMap, aga::ShaderParameterInfo& parameterInfo );

namespace aga
{
	class D3D11ShaderBase
	{
	public:
		const void* ByteCode() const { return m_byteCode; }
		size_t ByteCodeSize() const { return m_byteCodeSize; }

		const ShaderParameterInfo& GetParameterInfo() const
		{
			return m_parameterInfo;
		}

		D3D11ShaderBase( const void* byteCode, size_t byteCodeSize ) : m_byteCodeSize( byteCodeSize )
		{
			m_byteCode = new unsigned char[m_byteCodeSize];
			std::memcpy( m_byteCode, byteCode, m_byteCodeSize );
		}

		virtual ~D3D11ShaderBase()
		{
			delete[] m_byteCode;
		}

		D3D11ShaderBase( const D3D11ShaderBase& ) = delete;
		D3D11ShaderBase( D3D11ShaderBase&& other ) = default;
		D3D11ShaderBase& operator=( const D3D11ShaderBase& ) = delete;
		D3D11ShaderBase& operator=( D3D11ShaderBase&& other ) = default;

	protected:
		void* m_byteCode = nullptr;
		size_t m_byteCodeSize = 0;
		ShaderParameterInfo m_parameterInfo;
	};

	class D3D11VertexShader : public VertexShader, public D3D11ShaderBase
	{
	public:
		ID3D11VertexShader* Resource() { return m_pResource; }

		D3D11VertexShader( const void* byteCode, size_t byteCodeSize ) : D3D11ShaderBase( byteCode, byteCodeSize ) {}
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

	class D3D11GeometryShader : public GeometryShader, public D3D11ShaderBase
	{
	public:
		ID3D11GeometryShader* Resource() { return m_pResource; }

		D3D11GeometryShader( const void* byteCode, size_t byteCodeSize ) : D3D11ShaderBase( byteCode, byteCodeSize ) {}
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

	class D3D11PixelShader : public PixelShader, public D3D11ShaderBase
	{
	public:
		ID3D11PixelShader* Resource() { return m_pResource; }

		D3D11PixelShader( const void* byteCode, size_t byteCodeSize ) : D3D11ShaderBase( byteCode, byteCodeSize ) {}
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

	class D3D11ComputeShader : public ComputeShader, public D3D11ShaderBase
	{
	public:
		ID3D11ComputeShader* Resource() { return m_pResource; }

		D3D11ComputeShader( const void* byteCode, size_t byteCodeSize ) : D3D11ShaderBase( byteCode, byteCodeSize ) {}
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
