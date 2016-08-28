#pragma once

#include "common.h"
#include "IRenderResourceManager.h"

#include <map>
#include <memory>

struct ID3D11Resource;

class CRenderTargetManager : public IRenderTargetManager
{
public:
	virtual IRenderTarget* CreateRenderTarget( ID3D11Device* pDevice, const ITexture* pTexture, const D3D11_RENDER_TARGET_VIEW_DESC* rtvDesc, const String& renderTargetName ) override;
	virtual IDepthStencil* CreateDepthStencil( ID3D11Device* pDevice, const ITexture* pTexture, const D3D11_DEPTH_STENCIL_VIEW_DESC* dsvDesc, const String& depthStencilName ) override;
	virtual IRenderTarget* FindRenderTarget( const String& renderTargetName ) const override;
	virtual IDepthStencil* FindDepthStencil( const String& depthStencilName ) const override;

	virtual void SetRenderTarget( ID3D11DeviceContext* pDeviceContext, IRenderTarget* pRenderTarget, IDepthStencil* pDepthStencil ) override;
	virtual void SetRenderTarget( ID3D11DeviceContext* pDeviceContext, RenderTargetBinder& pBinder, IDepthStencil* pDepthStencil ) override;

private:
	virtual IRenderTarget* CreateRenderTarget( ID3D11Device* pDevice, ID3D11Resource* pResource, const D3D11_RENDER_TARGET_VIEW_DESC* rtvDesc, const String& renderTargetName );
	void RegisterRenderTarget( const String& renderTargetName, const std::shared_ptr<IRenderTarget>& renderTarget );
	void RegisterDepthStencil( const String& depthStencilName, const std::shared_ptr<IDepthStencil>& depthStencil );

	std::map<String, std::shared_ptr<IRenderTarget>> m_renderTargets;
	std::map<String, std::shared_ptr<IDepthStencil>> m_depthStencils;
};

