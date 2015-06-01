#pragma once

#include <D3D11.h>
#include <D3DX11.h>
#include "IShader.h"

class D3D11_CREATE_PS_TRAIT : public IShaderTrait
{
public:
	ID3D11Device* m_pDevice;
	TCHAR* m_pFilePath;
	char* m_pProfile;
};

class D3D11_SET_PS_TRAIT : public IShaderTrait
{
public:
	ID3D11DeviceContext* m_pDeviceContext;
};

class D3D11PixelShader : public IShader
{
private:
	ID3D11PixelShader* m_pPixelShader;

public:
	virtual bool CreateShader ( const D3D11_CREATE_PS_TRAIT& trait );
	virtual void SetShader ( const D3D11_SET_PS_TRAIT& trait );

	D3D11PixelShader ();
	virtual ~D3D11PixelShader ();
};

