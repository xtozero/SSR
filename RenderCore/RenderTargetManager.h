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
struct ID3D11DeviceContext;

class CRenderTargetManager
{
public:
	bool CreateRenderTarget( ID3D11Device* pDevice, ID3D11Resource* pResource, const D3D11_RENDER_TARGET_VIEW_DESC* rtvDesc, const String& renderTargetName );
	bool CreateDepthStencil( ID3D11Device* pDevice, ID3D11Resource* pResource, const D3D11_DEPTH_STENCIL_VIEW_DESC* dsvDesc, const String& depthStencilName );
	std::shared_ptr<IRenderTarget> GetRenderTarget( const String& renderTargetName );
	std::shared_ptr<IDepthStencil> GetDepthStencil( const String& depthStencilName );

	void SetRenderTarget( ID3D11DeviceContext* pDeviceContext, std::shared_ptr<IRenderTarget> pRenderTarget, std::shared_ptr<IDepthStencil> pDepthStencil );

private:
	void RegisterRenderTarget( const String& renderTargetName, const std::shared_ptr<IRenderTarget>& renderTarget );
	void RegisterDepthStencil( const String& depthStencilName, const std::shared_ptr<IDepthStencil>& depthStencil );
	bool IsRenderTargetExist( const String& renderTargetName );
	bool IsDepthStencilExist( const String& renderTargetName );

	std::map<String, std::shared_ptr<IRenderTarget>> m_renderTargets;
	std::map<String, std::shared_ptr<IDepthStencil>> m_depthStencils;
};

