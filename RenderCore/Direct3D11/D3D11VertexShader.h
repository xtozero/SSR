#pragma once

#include "../CommonRenderer/IShader.h"

#include <wrl/client.h>

struct D3D11_INPUT_ELEMENT_DESC;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11VertexShader;
struct ID3D11InputLayout;

class D3D11VertexShader : public IShader
{
public:
	virtual void SetShader( ) override;
	virtual void SetShaderResource( UINT slot, const IRenderResource* pResourceOrNull ) override;
	virtual void SetConstantBuffer( UINT slot, const IBuffer* pBufferOrNull ) override;

	D3D11_INPUT_ELEMENT_DESC* CreateInputElementDesc ( const UINT num );

	bool CreateShader( ID3D11Device& device, const void* byteCodePtr, const size_t byteCodeSize );

	D3D11VertexShader( ID3D11DeviceContext& deviceContext );
	virtual ~D3D11VertexShader ();

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_pVertexShader;

	D3D11_INPUT_ELEMENT_DESC* m_pInputElementDesc = nullptr;
	UINT m_numInputElement = 0;

	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_pInputLayout;
	ID3D11DeviceContext& m_deviceContext;
};

