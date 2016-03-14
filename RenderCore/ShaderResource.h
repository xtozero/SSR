#pragma once

#include "IShaderResource.h"

#include <wrl/client.h>

class CShaderResource : public IShaderResource
{
public:
	virtual bool LoadShaderResource( ID3D11Device* pDevice, const String& fileName ) override;
	virtual ID3D11ShaderResourceView* Get( ) const override;
	virtual void SetShaderResourceView( Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& shaderResourceView ) override;

	virtual bool IsDepthStencil( ) const override { return m_flag == SOURCE_RESOURCE_FLAG::DEPTH_STENCIL; }
	virtual bool IsRenderTarget( ) const override { return m_flag == SOURCE_RESOURCE_FLAG::RENDER_TARGET; }
	virtual bool IsTexture( ) const override { return m_flag == SOURCE_RESOURCE_FLAG::TEXTURE; }

	CShaderResource();
	CShaderResource( const int srcFlag );

private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pShaderResourceView;
	SOURCE_RESOURCE_FLAG m_flag;
};

