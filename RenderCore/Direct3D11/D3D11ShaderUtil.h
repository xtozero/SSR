#pragma once

#include <d3d11.h>
#include <D3Dcompiler.h>
#include <fstream>
#include <wrl/client.h>

#include "../common.h"
#include "../../shared/Util.h"

#pragma comment( lib, "d3dcompiler.lib" )

class CShaderByteCode
{
public:
	explicit CShaderByteCode( const size_t size );
	CShaderByteCode( const CShaderByteCode& byteCode ) = delete;
	CShaderByteCode( CShaderByteCode&& byteCode );
	CShaderByteCode& operator=( const CShaderByteCode& byteCode ) = delete;
	CShaderByteCode& operator=( CShaderByteCode&& byteCode );
	~CShaderByteCode( );

	BYTE* GetBufferPointer( ) const { return m_buffer; }
	size_t GetBufferSize( ) const { return m_size; }
private:
	BYTE* m_buffer;
	size_t m_size;
};

CShaderByteCode::CShaderByteCode( const size_t size ) :
	m_buffer( nullptr ),
	m_size( size )
{
	if ( m_size > 0 )
	{
		m_buffer = new BYTE[m_size];
	}
}

CShaderByteCode::CShaderByteCode( CShaderByteCode&& byteCode ) :
	m_buffer( byteCode.m_buffer ),
	m_size( byteCode.m_size )
{
	byteCode.m_buffer = nullptr;
	byteCode.m_size = 0;
}

CShaderByteCode & CShaderByteCode::operator=( CShaderByteCode&& byteCode )
{
	m_buffer = byteCode.m_buffer;
	m_size = byteCode.m_size;
	byteCode.m_buffer = nullptr;
	byteCode.m_size = 0;
	return *this;
}

CShaderByteCode::~CShaderByteCode( )
{
	delete[] m_buffer;
	m_buffer = nullptr;
	m_size = 0;
}

Microsoft::WRL::ComPtr<ID3DBlob> GetShaderBlob( const TCHAR* pFilePath, const char* pProfile )
{
	Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob( nullptr );
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob( nullptr );

	HRESULT hr = E_FAIL;

	int flag = D3DCOMPILE_ENABLE_STRICTNESS;

#ifdef _DEBUG
	flag |= D3DCOMPILE_DEBUG;
#endif

	std::ifstream shaderfile( pFilePath, std::ios::in | std::ios::ate );

	if ( shaderfile )
	{
		size_t size = static_cast<size_t>( shaderfile.tellg( ) );
		shaderfile.seekg( 0 );

		char* buffer = new char[size];
		shaderfile.read( buffer, size );

		D3DCompile( buffer,
					size,
					nullptr,
					nullptr,
					nullptr,
					"main",
					pProfile,
					flag,
					0,
					&shaderBlob,
					&errorBlob );

		if ( errorBlob )
		{
			char* errorMessage = static_cast<char*>( errorBlob->GetBufferPointer( ) );
			if ( errorMessage )
			{
				OutputDebugStringA( errorMessage );
			}
		}

		delete[] buffer;
	}

	return SUCCEEDED( hr ) ? shaderBlob : nullptr;
}

CShaderByteCode GetCompiledByteCode( const TCHAR* pFilePath )
{
	static String baseByteCodePath( _T( "../bin/Shader/" ) );
	String fileName = UTIL::GetFileName( pFilePath );
	String compiledByteCodePath = baseByteCodePath + fileName + String( _T( ".cso" ) );

	std::ifstream shaderfile( compiledByteCodePath, std::ios::in | std::ios::ate | std::ios::binary );

	if ( shaderfile )
	{
		size_t size = static_cast<size_t>( shaderfile.tellg( ) );
		shaderfile.seekg( 0 );

		CShaderByteCode byteCode( size );
		shaderfile.read( (char*)byteCode.GetBufferPointer( ), size );
		return byteCode;
	}

	return CShaderByteCode( 0 );
}
