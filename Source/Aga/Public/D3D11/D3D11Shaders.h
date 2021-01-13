#pragma once

#include "D3D11ResourceInterface.h"
#include "ShaderResource.h"
#include "ShaderPrameterInfo.h"

void ExtractShaderParameters( const void* byteCode, std::size_t byteCodeSize, ShaderParameterMap& parameterMap );

void BuildShaderParameterInfo( const std::map<std::string, ShaderParameter>& parameterMap, ShaderParameterInfo& parameterInfo );

template <typename ResourceType>
class CD3D11ShaderBase : public DeviceDependantBase<ResourceType>
{
public:
	const void* ByteCode( ) const { return m_byteCode; }
	std::size_t ByteCodeSize( ) const { return m_byteCodeSize; }

	const ShaderParameterInfo& GetParameterInfo( ) const 
	{
		return m_parameterInfo;
	}

	CD3D11ShaderBase( const void* byteCode, std::size_t byteCodeSize ) : m_byteCodeSize( byteCodeSize )
	{
		m_byteCode = new unsigned char[m_byteCodeSize];
		std::memcpy( m_byteCode, byteCode, m_byteCodeSize );

		ShaderParameterMap parameterMap;
		ExtractShaderParameters( byteCode, byteCodeSize, parameterMap );
		BuildShaderParameterInfo( parameterMap.GetParameterMap(), m_parameterInfo );
	}

	virtual ~CD3D11ShaderBase( )
	{
		delete m_byteCode;
	}

	CD3D11ShaderBase( const CD3D11ShaderBase& ) = delete;
	CD3D11ShaderBase( CD3D11ShaderBase&& other ) = default;
	CD3D11ShaderBase& operator=( const CD3D11ShaderBase& ) = delete;
	CD3D11ShaderBase& operator=( CD3D11ShaderBase&& other ) = default;

protected:
	void* m_byteCode = nullptr;
	std::size_t m_byteCodeSize = 0;
	ShaderParameterInfo m_parameterInfo;
};

class CD3D11VertexShader : public CD3D11ShaderBase<ID3D11VertexShader>
{
public:
	virtual void InitResource( ) override;

	CD3D11VertexShader( const void* byteCode, std::size_t byteCodeSize ) : CD3D11ShaderBase<ID3D11VertexShader>( byteCode, byteCodeSize ) {}
	CD3D11VertexShader( const CD3D11VertexShader& ) = delete;
	CD3D11VertexShader( CD3D11VertexShader&& ) = default;
	CD3D11VertexShader& operator=( const CD3D11VertexShader& ) = delete;
	CD3D11VertexShader& operator=( CD3D11VertexShader&& ) = default;
	~CD3D11VertexShader( ) = default;
};

class CD3D11GeometryShader : public CD3D11ShaderBase<ID3D11GeometryShader>
{
public:
	virtual void InitResource( ) override;

	CD3D11GeometryShader( const void* byteCode, std::size_t byteCodeSize ) : CD3D11ShaderBase<ID3D11GeometryShader>( byteCode, byteCodeSize ) {}
	CD3D11GeometryShader( const CD3D11GeometryShader& ) = delete;
	CD3D11GeometryShader( CD3D11GeometryShader&& ) = default;
	CD3D11GeometryShader& operator=( const CD3D11GeometryShader& ) = delete;
	CD3D11GeometryShader& operator=( CD3D11GeometryShader&& ) = default;
	~CD3D11GeometryShader( ) = default;
};

class CD3D11PixelShader : public CD3D11ShaderBase<ID3D11PixelShader>
{
public:
	virtual void InitResource( ) override;

	CD3D11PixelShader( const void* byteCode, std::size_t byteCodeSize ) : CD3D11ShaderBase<ID3D11PixelShader>( byteCode, byteCodeSize ) {}
	CD3D11PixelShader( const CD3D11PixelShader& ) = delete;
	CD3D11PixelShader( CD3D11PixelShader&& ) = default;
	CD3D11PixelShader& operator=( const CD3D11PixelShader& ) = delete;
	CD3D11PixelShader& operator=( CD3D11PixelShader&& ) = default;
	~CD3D11PixelShader( ) = default;
};

class CD3D11ComputeShader : public CD3D11ShaderBase<ID3D11ComputeShader>
{
public:
	virtual void InitResource( ) override;

	CD3D11ComputeShader( const void* byteCode, std::size_t byteCodeSize ) : CD3D11ShaderBase<ID3D11ComputeShader>( byteCode, byteCodeSize ) {}
	CD3D11ComputeShader( const CD3D11ComputeShader& ) = delete;
	CD3D11ComputeShader( CD3D11ComputeShader&& ) = default;
	CD3D11ComputeShader& operator=( const CD3D11ComputeShader& ) = delete;
	CD3D11ComputeShader& operator=( CD3D11ComputeShader&& ) = default;
	~CD3D11ComputeShader( ) = default;
};

template <typename ResourceType>
class D3D11ShaderBase
{
public:
	const void* ByteCode( ) const { return m_byteCode; }
	std::size_t ByteCodeSize( ) const { return m_byteCodeSize; }

	const ShaderParameterInfo& GetParameterInfo( ) const
	{
		return m_parameterInfo;
	}

	D3D11ShaderBase( const void* byteCode, std::size_t byteCodeSize ) : m_byteCodeSize( byteCodeSize )
	{
		m_byteCode = new unsigned char[m_byteCodeSize];
		std::memcpy( m_byteCode, byteCode, m_byteCodeSize );

		ShaderParameterMap parameterMap;
		ExtractShaderParameters( byteCode, byteCodeSize, parameterMap );
		BuildShaderParameterInfo( parameterMap.GetParameterMap( ), m_parameterInfo );
	}

	virtual ~D3D11ShaderBase( )
	{
		delete[] m_byteCode;
		Free( );
	}

	D3D11ShaderBase( const D3D11ShaderBase& ) = delete;
	D3D11ShaderBase( D3D11ShaderBase&& other ) = default;
	D3D11ShaderBase& operator=( const D3D11ShaderBase& ) = delete;
	D3D11ShaderBase& operator=( D3D11ShaderBase&& other ) = default;

protected:
	void Free( )
	{
		if ( m_pResource )
		{
			m_pResource->Release( );
			m_pResource = nullptr;
		}
	}

	void* m_byteCode = nullptr;
	std::size_t m_byteCodeSize = 0;
	ResourceType* m_pResource = nullptr;
	ShaderParameterInfo m_parameterInfo;
};

class D3D11VertexShader : public aga::VertexShader, public D3D11ShaderBase<ID3D11VertexShader>
{
public:
	virtual void InitResource( ) override;
	virtual void Free( ) override;

	D3D11VertexShader( const void* byteCode, std::size_t byteCodeSize ) : D3D11ShaderBase<ID3D11VertexShader>( byteCode, byteCodeSize ) {}
	D3D11VertexShader( const D3D11VertexShader& ) = delete;
	D3D11VertexShader( D3D11VertexShader&& ) = default;
	D3D11VertexShader& operator=( const D3D11VertexShader& ) = delete;
	D3D11VertexShader& operator=( D3D11VertexShader&& ) = default;
	~D3D11VertexShader( ) = default;
};

class D3D11PixelShader : public aga::PixelShader, public D3D11ShaderBase<ID3D11PixelShader>
{
public:
	virtual void InitResource( ) override;
	virtual void Free( ) override;

	D3D11PixelShader( const void* byteCode, std::size_t byteCodeSize ) : D3D11ShaderBase<ID3D11PixelShader>( byteCode, byteCodeSize ) {}
	D3D11PixelShader( const D3D11PixelShader& ) = delete;
	D3D11PixelShader( D3D11PixelShader&& ) = default;
	D3D11PixelShader& operator=( const D3D11PixelShader& ) = delete;
	D3D11PixelShader& operator=( D3D11PixelShader&& ) = default;
	~D3D11PixelShader( ) = default;
};