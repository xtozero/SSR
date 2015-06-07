#include "stdafx.h"
#include "D3D11BaseShader.h"

ID3D10Blob* D3D11BaseShader::GetShaderBlob ( const TCHAR* pFilePath, const char* pProfile )
{
	ID3D10Blob* shaderBlob = NULL;

	HRESULT hr;

	D3DX11CompileFromFile ( pFilePath,
		NULL,
		NULL,
		"main",
		pProfile,
		0,
		0,
		NULL,
		&shaderBlob,
		NULL,
		&hr );

	return SUCCEEDED ( hr ) ? shaderBlob : NULL;
}

D3D11BaseShader::D3D11BaseShader ()
{
}


D3D11BaseShader::~D3D11BaseShader ()
{
}
