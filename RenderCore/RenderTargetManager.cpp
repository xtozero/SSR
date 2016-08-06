#include "stdafx.h"

#include "DepthStencil.h"
#include "RenderTarget.h"
#include "RenderTargetManager.h"
#include "ITexture.h"

#include <d3d11.h>

IRenderTarget* CRenderTargetManager::CreateRenderTarget( ID3D11Device* pDevice, ID3D11Resource* pResource, const D3D11_RENDER_TARGET_VIEW_DESC* rtvDesc, const String& renderTargetName )
{
	if ( auto renderTarget = FindRenderTarget( renderTargetName ) )
	{
		return renderTarget;
	}

	if ( pDevice && pResource )
	{
		std::shared_ptr<IRenderTarget> newRenderTarget = std::make_shared<CRenderTarget>( );

		if ( newRenderTarget && newRenderTarget->CreateRenderTarget( pDevice, pResource, rtvDesc ) )
		{
			RegisterRenderTarget( renderTargetName, newRenderTarget );
			return newRenderTarget.get( );
		}
	}

	return nullptr;
}

IDepthStencil* CRenderTargetManager::CreateDepthStencil( ID3D11Device* pDevice, const ITexture* pTexture, const D3D11_DEPTH_STENCIL_VIEW_DESC* dsvDesc, const String& depthStencilName )
{
	if ( auto depthStencil = FindDepthStencil( depthStencilName ) )
	{
		return depthStencil;
	}

	if ( pDevice && pTexture )
	{
		std::shared_ptr<IDepthStencil> newDepthStencil = std::make_shared<CDepthStencil>( );

		if ( newDepthStencil && newDepthStencil->CreateDepthStencil( pDevice, pTexture, dsvDesc ) )
		{
			RegisterDepthStencil( depthStencilName, newDepthStencil );
			return newDepthStencil.get( );
		}
	}

	return nullptr;
}

IRenderTarget* CRenderTargetManager::FindRenderTarget( const String& renderTargetName ) const
{
	auto found = m_renderTargets.find( renderTargetName );

	if ( found != m_renderTargets.end( ) )
	{
		return found->second.get( );
	}

	return nullptr;
}

IDepthStencil* CRenderTargetManager::FindDepthStencil( const String& depthStencilName ) const
{
	auto found = m_depthStencils.find( depthStencilName );

	if ( found != m_depthStencils.end( ) )
	{
		return found->second.get( );
	}

	return nullptr;
}

void CRenderTargetManager::SetRenderTarget( ID3D11DeviceContext* pDeviceContext, IRenderTarget* pRenderTarget, IDepthStencil* pDepthStencil )
{
	if ( pDeviceContext )
	{
		ID3D11RenderTargetView* rtv = pRenderTarget ? pRenderTarget->Get( ) : nullptr;
		ID3D11DepthStencilView* dsv = pDepthStencil ? pDepthStencil->Get( ) : nullptr;

		pDeviceContext->OMSetRenderTargets( 1, &rtv, dsv );
	}
}

void CRenderTargetManager::SetRenderTarget( ID3D11DeviceContext* pDeviceContext, RenderTargetBinder& binder, IDepthStencil* pDepthStencil )
{
	if ( pDeviceContext )
	{
		ID3D11DepthStencilView* dsv = pDepthStencil ? pDepthStencil->Get( ) : nullptr;

		pDeviceContext->OMSetRenderTargets( binder.Count(), binder.Get( ), dsv );
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