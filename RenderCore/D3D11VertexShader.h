#pragma once

#include "D3D11BaseShader.h"

class D3D11VertexShader : public D3D11BaseShader
{
private: 
	ID3D11VertexShader* m_pVertexShader;

	D3D11_INPUT_ELEMENT_DESC* m_pInputElementDesc;
	UINT m_numInputElement;

	ID3D11InputLayout* m_pInputLayout;

public:
	virtual bool CreateShader ( ID3D11Device* pDevice, const TCHAR* pFilePath, const char* pProfile );
	virtual void SetShader ( ID3D11DeviceContext* pDeviceContext );

	D3D11_INPUT_ELEMENT_DESC* CreateInputElementDesc ( const UINT num );

	D3D11VertexShader ();
	virtual ~D3D11VertexShader ();
};
