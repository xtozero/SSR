#pragma once

#include "ITexture.h"

#include <wrl/client.h>

struct ID3D11Device;
struct ID3D11Texture2D;

class CTexture2D : public ITexture
{
public:
	virtual bool LoadFromFile( ID3D11Device* pDevice, const String& fileName ) override;
	virtual ID3D11Resource* Get( ) const override;
	virtual bool SetTexture( Microsoft::WRL::ComPtr<ID3D11Resource>& pTexture ) override;
	virtual bool Create( ID3D11Device* pDevice, const D3D11_TEXTURE2D_DESC& desc, const D3D11_SUBRESOURCE_DATA* pInitialData = nullptr ) override;
	virtual TEXTURE_TYPE GetType( ) const override { return TEXTURE_TYPE::TEXTURE_2D; }

	CTexture2D( );

private:
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pTexture2D;
};

