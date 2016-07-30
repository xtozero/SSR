#include "stdafx.h"
#include "IShader.h"

#include "../Shared/Util.h"

#include <D3DX11.h>
#include <fstream>

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

CShaderByteCode & CShaderByteCode::operator=( CShaderByteCode && byteCode )
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

bool IShader::CreateShader( ID3D11Device* pDevice, const TCHAR* pFilePath, const char* pProfile )
{
	if ( pDevice )
	{
		CShaderByteCode byteCode = GetCompiledByteCode( pFilePath );
		if ( byteCode.GetBufferSize( ) > 0 )
		{
			return CreateShaderInternal( pDevice, byteCode.GetBufferPointer( ), byteCode.GetBufferSize( ) );
		}
		else
		{
			Microsoft::WRL::ComPtr<ID3D10Blob> shaderBlob = GetShaderBlob( pFilePath, pProfile );

			if ( shaderBlob )
			{
				return CreateShaderInternal( pDevice, shaderBlob->GetBufferPointer( ), shaderBlob->GetBufferSize( ) );
			}
		}
	}

	return false;
}

Microsoft::WRL::ComPtr<ID3D10Blob> IShader::GetShaderBlob( const TCHAR* pFilePath, const char* pProfile )
{
	Microsoft::WRL::ComPtr<ID3D10Blob> shaderBlob( nullptr );
	Microsoft::WRL::ComPtr<ID3D10Blob> errorBlob( nullptr );

	HRESULT hr;

	int flag = D3D10_SHADER_ENABLE_STRICTNESS;

#ifdef _DEBUG
	flag |= D3D10_SHADER_DEBUG;
#endif

	D3DX11CompileFromFile( pFilePath,
		nullptr,
		nullptr,
		"main",
		pProfile,
		flag,
		0,
		nullptr,
		&shaderBlob,
		&errorBlob,
		&hr );

	if ( errorBlob )
	{
		char* errorMessage = static_cast<char*>(errorBlob->GetBufferPointer( ));
		if ( errorMessage )
		{
			OutputDebugStringA( errorMessage );
		}
	}

	return SUCCEEDED( hr ) ? shaderBlob : nullptr;
}

CShaderByteCode IShader::GetCompiledByteCode( const TCHAR* pFilePath )
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
