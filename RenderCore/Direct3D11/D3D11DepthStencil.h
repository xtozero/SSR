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

	bool CreateDepthStencil( ID3D11Device& device, const ITexture& texture, const TEXTURE_TRAIT* traitOrNull );
	void SetRenderTargetView( Microsoft::WRL::ComPtr<ID3D11DepthStencilView>& depthStencilView );
	void Clear( ID3D11DeviceContext& deviceContext, unsigned int clearFlag, float depth, unsigned char stencil );

private:
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pDepthStencilVeiw;
};

