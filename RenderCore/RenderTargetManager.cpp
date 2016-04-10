#include "stdafx.h"

#include "DepthStencil.h"
#include "RenderTarget.h"
#include "RenderTargetManager.h"
#include "ITexture.h"

#include <d3d11.h>

bool CRenderTargetManager::CreateRenderTarget( ID3D11Device* pDevice, ID3D11Resource* pResource, const D3D11_RENDER_TARGET_VIEW_DESC* rtvDesc, const String& renderTargetName )
{
	if ( FindRenderTarget( renderTargetName ) )
	{
		return true;
	}

	if ( pDevice && pResource )
	{
		std::shared_ptr<IRenderTarget> newRenderTarget = std::make_shared<CRenderTarget>( );

		if ( newRenderTarget && newRenderTarget->CreateRenderTarget( pDevice, pResource, rtvDesc ) )
		{
			RegisterRenderTarget( renderTargetName, newRenderTarget );
			return true;
		}
	}

	return false;
}

bool CRenderTargetManager::CreateDepthStencil( ID3D11Device* pDevice, std::shared_ptr<ITexture>& pTexture, const D3D11_DEPTH_STENCIL_VIEW_DESC* dsvDesc, const String& depthStencilName )
{
	if ( FindDepthStencil( depthStencilName ) )
	{
		return true;
	}

	if ( pDevice && pTexture )
	{
		std::shared_ptr<IDepthStencil> newDepthStencil = std::make_shared<CDepthStencil>( );

		if ( newDepthStencil && newDepthStencil->CreateDepthStencil( pDevice, pTexture, dsvDesc ) )
		{
			RegisterDepthStencil( depthStencilName, newDepthStencil );
			return true;
		}
	}

	return false;
}

std::shared_ptr<IRenderTarget> CRenderTargetManager::FindRenderTarget( const String& renderTargetName ) const
{
	auto founded = m_renderTargets.find( renderTargetName );

	if ( founded != m_renderTargets.end( ) )
	{
		return founded->second;
	}

	return nullptr;
}

std::shared_ptr<IDepthStencil> CRenderTargetManager::FindDepthStencil( const String& depthStencilName ) const
{
	auto founded = m_depthStencils.find( depthStencilName );

	if ( founded != m_depthStencils.end( ) )
	{
		return founded->second;
	}

	return nullptr;
}

void CRenderTargetManager::SetRenderTarget( ID3D11DeviceContext* pDeviceContext, std::shared_ptr<IRenderTarget> pRenderTarget, std::shared_ptr<IDepthStencil> pDepthStencil )
{
	if ( pDeviceContext )
	{
		ID3D11RenderTargetView* rtv = pRenderTarget ? pRenderTarget->Get( ) : nullptr;
		ID3D11DepthStencilView* dsv = pDepthStencil ? pDepthStencil->Get( ) : nullptr;

		pDeviceContext->OMSetRenderTargets( 1, &rtv, dsv );
	}
}

void CRenderTargetManager::RegisterRenderTarget( const String& renderTargetName, const std::shared_ptr<IRenderTarget>& renderTarget )
{
	m_renderTargets.emplace( renderTargetName, renderTarget );
}

void CRenderTargetManager::RegisterDepthStencil( const String& depthStencilName, const std::shared_ptr<IDepthStencil>& depthStencil )
{
	m_depthStencils.emplace( depthStencilName, depthStencil );
}