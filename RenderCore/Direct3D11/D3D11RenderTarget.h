#pragma once

#include "../CommonRenderer/IRenderResource.h"

#include <wrl/client.h>

struct ID3D11Device;
struct ID3D11Resource;
struct TEXTURE_TRAIT;

class CRenderTarget : public IRenderResource
{
public:
	virtual void* Get( ) const override;

	bool CreateRenderTarget( ID3D11Device* pDevice, ID3D11Resource* pResource, const TEXTURE_TRAIT* traitOrNull );
	void SetRenderTargetView( Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& renderTargetView );

private:
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;
};

