#pragma once

#include "IRenderTarget.h"

#include <wrl/client.h>

class CRenderTarget : public IRenderTarget
{
public:
	virtual bool CreateRenderTarget( ID3D11Device* pDevice, ID3D11Resource* pResource, const D3D11_RENDER_TARGET_VIEW_DESC* rtvDesc ) override;
	virtual ID3D11RenderTargetView* Get( ) const override;
	virtual void SetRenderTargetView( Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& renderTargetView ) override;

	virtual void Clear( ID3D11DeviceContext* pDeviceContext, const float (&clearColor)[4] );

	CRenderTarget( );
private:
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;
};

