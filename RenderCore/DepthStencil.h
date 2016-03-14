#pragma once

#include "IDepthStencil.h"

#include <wrl/client.h>

class CDepthStencil : public IDepthStencil
{
public:
	virtual bool CreateDepthStencil( ID3D11Device* pDevice, ID3D11Resource* pResource, const D3D11_DEPTH_STENCIL_VIEW_DESC* dsvDesc ) override;
	virtual ID3D11DepthStencilView* Get( ) const override;
	virtual void SetRenderTargetView( Microsoft::WRL::ComPtr<ID3D11DepthStencilView>& depthStencilView ) override;
	virtual void Clear( ID3D11DeviceContext* pDeviceContext, unsigned int clearFlag, float depth, unsigned char stencil ) override;

private:
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pDepthStencilVeiw;
};

