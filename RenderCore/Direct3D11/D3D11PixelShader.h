#pragma once

#include "../IShader.h"

struct ID3D11DeviceContext;
struct ID3D11PixelShader;

class D3D11PixelShader : public IShader
{
public:
	virtual void SetShader( ) override;
	virtual void SetShaderResource( UINT slot, const IRenderResource* pResource ) override;
	virtual void SetConstantBuffer( UINT slot, const IBuffer* pBuffer ) override;

	D3D11PixelShader( ID3D11DeviceContext* pDeviceContext );
protected:
	virtual bool CreateShaderInternal( ID3D11Device* pDevice, const void* byteCodePtr, const size_t byteCodeSize );

private:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pPixelShader;
	ID3D11DeviceContext* m_pDeviceContext = nullptr;
};

