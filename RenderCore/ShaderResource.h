#pragma once

#include "IShaderResource.h"

#include <wrl/client.h>

class CShaderResource : public IShaderResource
{
public:
	virtual bool LoadShaderResource( ID3D11Device* pDevice, const String& fileName ) override;
	virtual ID3D11ShaderResourceView* Get( ) const override;
	virtual void SetShaderResourceView( Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& shaderResourceView ) override;
	virtual bool CreateShaderResource( ID3D11Device* pDevice, const ITexture* pTexture, const CShaderResourceViewDescription* desc ) override;

	CShaderResource();

private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pShaderResourceView;
};

