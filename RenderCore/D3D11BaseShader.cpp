#include "stdafx.h"
#include "D3D11BaseShader.h"

#include <D3DX11.h>

Microsoft::WRL::ComPtr<ID3D10Blob>D3D11BaseShader::GetShaderBlob( const TCHAR* pFilePath, const char* pProfile )
{
	Microsoft::WRL::ComPtr<ID3D10Blob> shaderBlob( nullptr );

	HRESULT hr;

	D3DX11CompileFromFile ( pFilePath,
		nullptr,
		nullptr,
		"main",
		pProfile,
		0,
		0,
		nullptr,
		&shaderBlob,
		nullptr,
		&hr );

	return SUCCEEDED( hr ) ? shaderBlob : nullptr;
}

D3D11BaseShader::D3D11BaseShader ()
{
}


D3D11BaseShader::~D3D11BaseShader ()
{
}
