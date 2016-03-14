#pragma once

#include "common.h"

struct ID3D11Device;
struct ID3D11RenderTargetView;
struct ID3D11Resource;
struct D3D11_RENDER_TARGET_VIEW_DESC;
struct ID3D11DeviceContext;

namespace Microsoft
{
	namespace WRL
	{
		template <typename T> class ComPtr;
	}
}

class IRenderTarget
{
public:
	virtual bool CreateRenderTarget( ID3D11Device* pDevice, ID3D11Resource* pResource, const D3D11_RENDER_TARGET_VIEW_DESC* rtvDesc ) = 0;
	virtual ID3D11RenderTargetView* Get( ) const = 0;
	virtual void SetRenderTargetView( Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& renderTargetView ) = 0;

	virtual void Clear( ID3D11DeviceContext* pDeviceContext, const float clearColor[4] ) = 0;

	virtual ~IRenderTarget( ) = default;

protected:
	IRenderTarget( ) = default;
};
