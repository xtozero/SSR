#pragma once

#include "../CommonRenderer/IRenderResource.h"

#include <wrl/client.h>

struct ID3D11Device;
struct ID3D11DepthStencilView;
struct ID3D11DeviceContext;
struct TEXTURE_TRAIT;

class ITexture;

class CDepthStencil : public IRenderResource
{
public:
	virtual void* Get( ) const override;

	bool CreateDepthStencil( ID3D11Device* pDevice, const ITexture* pResource, const TEXTURE_TRAIT* traitOrNull );
	void SetRenderTargetView( Microsoft::WRL::ComPtr<ID3D11DepthStencilView>& depthStencilView );
	void Clear( ID3D11DeviceContext* pDeviceContext, unsigned int clearFlag, float depth, unsigned char stencil );

	CDepthStencil( );
private:
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pDepthStencilVeiw;
};

