#pragma once

#include "IShader.h"

class D3D11BaseShader : public IShader
{
protected:
	ID3D10Blob* GetShaderBlob ( const TCHAR* pFilePath, const char* pProfile );

public:
	D3D11BaseShader ();
	virtual ~D3D11BaseShader ();
};

