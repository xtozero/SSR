#pragma once

#include "D3D11BaseShader.h"

class D3D11PixelShader : public D3D11BaseShader
{
private:
	ID3D11PixelShader* m_pPixelShader;

public:
	virtual bool CreateShader ( ID3D11Device* pDevice, const TCHAR* pFilePath, const char* pProfile );
	virtual void SetShader ( ID3D11DeviceContext* pDeviceContext );

	D3D11PixelShader ();
	virtual ~D3D11PixelShader ();
};

