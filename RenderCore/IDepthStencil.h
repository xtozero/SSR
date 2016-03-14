#pragma once


namespace Microsoft
{
	namespace WRL
	{
		template <typename T> class ComPtr;
	}
}

struct ID3D11Device;
struct ID3D11Resource;
struct D3D11_DEPTH_STENCIL_VIEW_DESC;
struct ID3D11DepthStencilView;
struct ID3D11DeviceContext;

class IDepthStencil
{
public:
	virtual bool CreateDepthStencil( ID3D11Device* pDevice, ID3D11Resource* pResource, const D3D11_DEPTH_STENCIL_VIEW_DESC* dsvDesc ) = 0;
	virtual ID3D11DepthStencilView* Get( ) const = 0;
	virtual void SetRenderTargetView( Microsoft::WRL::ComPtr<ID3D11DepthStencilView>& depthStencilView ) = 0;
	virtual void Clear( ID3D11DeviceContext* pDeviceContext, unsigned int clearFlag, float depth, unsigned char stencil ) = 0;

	virtual ~IDepthStencil( ) = default;

protected:
	IDepthStencil( ) = default;
};