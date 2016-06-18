#pragma once

#include "common.h"

#include <map>
#include <memory>

struct ID3D11Device;
struct ID3D11Resource;
struct D3D11_RENDER_TARGET_VIEW_DESC;
struct D3D11_DEPTH_STENCIL_VIEW_DESC;
class IRenderTarget;
class IDepthStencil;
class ITexture;
struct ID3D11DeviceContext;

class CRenderTargetManager
{
public:
	IRenderTarget* CreateRenderTarget( ID3D11Device* pDevice, ID3D11Resource* pResource, const D3D11_RENDER_TARGET_VIEW_DESC* rtvDesc, const String& renderTargetName );
	IDepthStencil* CreateDepthStencil( ID3D11Device* pDevice, const ITexture* pTexture, const D3D11_DEPTH_STENCIL_VIEW_DESC* dsvDesc, const String& depthStencilName );
	IRenderTarget* FindRenderTarget( const String& renderTargetName ) const;
	IDepthStencil* FindDepthStencil( const String& depthStencilName ) const;

	void SetRenderTarget( ID3D11DeviceContext* pDeviceContext, IRenderTarget* pRenderTarget, IDepthStencil* pDepthStencil );

private:
	void RegisterRenderTarget( const String& renderTargetName, const std::shared_ptr<IRenderTarget>& renderTarget );
	void RegisterDepthStencil( const String& depthStencilName, const std::shared_ptr<IDepthStencil>& depthStencil );

	std::map<String, std::shared_ptr<IRenderTarget>> m_renderTargets;
	std::map<String, std::shared_ptr<IDepthStencil>> m_depthStencils;
};

