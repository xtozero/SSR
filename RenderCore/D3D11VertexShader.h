#pragma once

#include <D3D11.h>
#include <D3DX11.h>
#include "IShader.h"

class D3D11_CREATE_VS_TRAIT : public IShaderTrait
{
public:
	ID3D11Device* m_pDevice;
	TCHAR* m_pFilePath;
	char* m_pProfile;
};

class D3D11_SET_VS_TRAIT : public IShaderTrait
{
public:
	ID3D11DeviceContext* m_pDeviceContext;
};

class D3D11VertexShader : public IShader
{
private: 
	ID3D11VertexShader* m_pVertexShader;
	ID3D10Blob* m_shaderBlob;

public:
	virtual bool CreateShader ( const D3D11_CREATE_VS_TRAIT& trait );
	virtual void SetShader ( const D3D11_SET_VS_TRAIT& trait );

	D3D11VertexShader ();
	virtual ~D3D11VertexShader ();
};

