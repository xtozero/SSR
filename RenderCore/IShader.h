#pragma once

#include <wrl/client.h>
#include <D3D11.h>
#include <D3DX11.h>
#include "common.h"
#include <memory>

class IShader
{
public:
	virtual bool CreateShader ( ID3D11Device* pDevice, const TCHAR* pFilePath, const char* pProfile ) = 0;
	virtual void SetShader ( ID3D11DeviceContext* pDeviceContext ) = 0;
	virtual void SetShaderResource( ID3D11DeviceContext* pDeviceContext, UINT slot, ID3D11ShaderResourceView* pResource ) = 0;

protected:
	IShader () {}

public:
	virtual ~IShader () {}
};