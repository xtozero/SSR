#pragma once

#include "D3D11BaseShader.h"

struct ID3D11PixelShader;

class D3D11PixelShader : public D3D11BaseShader
{
private:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pPixelShader;

public:
	virtual bool CreateShader( ID3D11Device* pDevice, const TCHAR* pFilePath, const char* pProfile ) override;
	virtual void SetShader( ID3D11DeviceContext* pDeviceContext ) override;
	virtual void SetShaderResource( ID3D11DeviceContext* pDeviceContext, UINT slot, const IShaderResource* pResource ) override;
	virtual void SetConstantBuffer( ID3D11DeviceContext* pDeviceContext, UINT slot, const IBuffer* pBuffer ) override;

	D3D11PixelShader ();
	virtual ~D3D11PixelShader ();
};

