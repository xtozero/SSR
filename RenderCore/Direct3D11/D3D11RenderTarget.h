#pragma once

#include "../CommonRenderer/IRenderResource.h"

#include <wrl/client.h>

struct ID3D11Device;
struct TEXTURE_TRAIT;

class CRenderTarget : public IRenderResource
{
public:
	virtual void* Get( ) const override;

	bool CreateRenderTarget( ID3D11Device& device, const ITexture& texture, const TEXTURE_TRAIT* traitOrNull );
	void SetRenderTargetView( Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& renderTargetView );

private:
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;
};

