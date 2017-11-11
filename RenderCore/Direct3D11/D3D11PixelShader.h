#pragma once

#include "../CommonRenderer/IShader.h"

#include <wrl/client.h>

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11PixelShader;

class D3D11PixelShader : public IShader
{
public:
	virtual void SetShader( ) override;
	virtual void SetShaderResource( UINT slot, const IRenderResource* pResource ) override;

	bool CreateShader( ID3D11Device& device, const void* byteCodePtr, const size_t byteCodeSize );

	D3D11PixelShader( ID3D11DeviceContext& deviceContext );

private:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pPixelShader;
	ID3D11DeviceContext& m_deviceContext;
};

