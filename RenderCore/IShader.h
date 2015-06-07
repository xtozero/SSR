#pragma once

#include <D3D11.h>
#include <D3DX11.h>
#include "common.h"

class IShader
{
public:
	virtual bool CreateShader ( ID3D11Device* pDevice, const TCHAR* pFilePath, const char* pProfile ) = 0;
	virtual void SetShader ( ID3D11DeviceContext* pDeviceContext ) = 0;

protected:
	IShader () {}

public:
	virtual ~IShader () {}
};