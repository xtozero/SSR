#pragma once

#include "common.h"

#include <memory>

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11ShaderResourceView;
class IShaderResource;
class IBuffer;

class IShader
{
public:
	virtual bool CreateShader ( ID3D11Device* pDevice, const TCHAR* pFilePath, const char* pProfile ) = 0;
	virtual void SetShader ( ID3D11DeviceContext* pDeviceContext ) = 0;
	virtual void SetShaderResource( ID3D11DeviceContext* pDeviceContext, UINT slot, const IShaderResource* pResource ) = 0;
	virtual void SetConstantBuffer( ID3D11DeviceContext* pDeviceContext, UINT slot, const IBuffer* pBuffer ) = 0;

protected:
	IShader () = default;

public:
	virtual ~IShader( ) = default;
};