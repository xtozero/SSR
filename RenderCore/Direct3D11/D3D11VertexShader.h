#pragma once

#include "../CommonRenderer/IShader.h"

#include <wrl/client.h>

class CD3D11RenderStateManager;

struct D3D11_INPUT_ELEMENT_DESC;
struct ID3D11Device;
struct ID3D11VertexShader;
struct ID3D11InputLayout;

class D3D11VertexShader : public IShader
{
public:
	virtual void SetShader( ) override;
	virtual void SetShaderResource( UINT slot, const IRenderResource* pResourceOrNull ) override;

	D3D11_INPUT_ELEMENT_DESC* CreateInputElementDesc ( const UINT num );

	bool CreateShader( ID3D11Device& device, const void* byteCodePtr, const size_t byteCodeSize );

	D3D11VertexShader( CD3D11RenderStateManager& renderStateManager );
	virtual ~D3D11VertexShader ();

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_pVertexShader;

	D3D11_INPUT_ELEMENT_DESC* m_pInputElementDesc = nullptr;
	UINT m_numInputElement = 0;

	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_pInputLayout;
	CD3D11RenderStateManager& m_renderStateManager;
};

