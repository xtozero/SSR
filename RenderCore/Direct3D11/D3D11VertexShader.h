#pragma once

#include "../IShader.h"

struct D3D11_INPUT_ELEMENT_DESC;
struct ID3D11DeviceContext;
struct ID3D11VertexShader;
struct ID3D11InputLayout;


class D3D11VertexShader : public IShader
{
public:
	virtual void SetShader( ) override;
	virtual void SetShaderResource( UINT slot, const IRenderResource* pResource ) override;
	virtual void SetConstantBuffer( UINT slot, const IBuffer* pBuffer ) override;

	D3D11_INPUT_ELEMENT_DESC* CreateInputElementDesc ( const UINT num );

	D3D11VertexShader( ID3D11DeviceContext* pDeviceContext );
	virtual ~D3D11VertexShader ();

protected:
	virtual bool CreateShaderInternal( ID3D11Device* pDevice, const void* byteCodePtr, const size_t byteCodeSize );

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_pVertexShader;

	D3D11_INPUT_ELEMENT_DESC* m_pInputElementDesc = nullptr;
	UINT m_numInputElement = 0;

	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_pInputLayout;
	ID3D11DeviceContext* m_pDeviceContext = nullptr;
};

