#pragma once

#include "IShader.h"

struct ID3D11PixelShader;

class D3D11PixelShader : public IShader
{
public:
	virtual void SetShader( ID3D11DeviceContext* pDeviceContext ) override;
	virtual void SetShaderResource( ID3D11DeviceContext* pDeviceContext, UINT slot, const IShaderResource* pResource ) override;
	virtual void SetConstantBuffer( ID3D11DeviceContext* pDeviceContext, UINT slot, const IBuffer* pBuffer ) override;

	D3D11PixelShader ();
	virtual ~D3D11PixelShader ();
protected:
	virtual bool CreateShaderInternal( ID3D11Device* pDevice, const void* byteCodePtr, const size_t byteCodeSize );

private:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pPixelShader;
};

