#include "stdafx.h"
#include "D3D11BaseShader.h"

#include <D3DX11.h>

Microsoft::WRL::ComPtr<ID3D10Blob>D3D11BaseShader::GetShaderBlob( const TCHAR* pFilePath, const char* pProfile )
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
		char* errorMessage = static_cast<char*>( errorBlob->GetBufferPointer( ) );
		if ( errorMessage )
		{
			OutputDebugStringA( errorMessage );
		}
	}

	return SUCCEEDED( hr ) ? shaderBlob : nullptr;
}

D3D11BaseShader::D3D11BaseShader ()
{
}


D3D11BaseShader::~D3D11BaseShader ()
{
}
