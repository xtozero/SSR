#pragma once

#include "IShader.h"

struct ID3D11VertexShader;
struct D3D11_INPUT_ELEMENT_DESC;
struct ID3D11InputLayout;


class D3D11VertexShader : public IShader
{
public:
	virtual void SetShader( ID3D11DeviceContext* pDeviceContext ) override;
	virtual void SetShaderResource( ID3D11DeviceContext* pDeviceContext, UINT slot, const IShaderResource* pResource ) override;
	virtual void SetConstantBuffer( ID3D11DeviceContext* pDeviceContext, UINT slot, const IBuffer* pBuffer ) override;

	D3D11_INPUT_ELEMENT_DESC* CreateInputElementDesc ( const UINT num );

	D3D11VertexShader ();
	virtual ~D3D11VertexShader ();

protected:
	virtual bool CreateShaderInternal( ID3D11Device* pDevice, const void* byteCodePtr, const size_t byteCodeSize );

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_pVertexShader;

	D3D11_INPUT_ELEMENT_DESC* m_pInputElementDesc;
	UINT m_numInputElement;

	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_pInputLayout;

};

