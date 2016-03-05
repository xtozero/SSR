#pragma once

#include "ITexture.h"

class CTexture : public ITexture
{
public:
	virtual bool LoadTexture( ID3D11Device* pDevice, const String& fileName ) override;
	virtual ID3D11ShaderResourceView* GetResource( ) override;

	CTexture( );

private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pTextureView;
};

