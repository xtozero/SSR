#include "stdafx.h"

#include "DepthStencil.h"
#include "RenderTarget.h"
#include "RenderTargetManager.h"
#include "ITexture.h"

#include <d3d11.h>

IRenderTarget* CRenderTargetManager::CreateRenderTarget( ID3D11Device* pDevice, const ITexture* pTexture, const D3D11_RENDER_TARGET_VIEW_DESC* rtvDesc, const String& renderTargetName )
{
	if ( pTexture == nullptr )
	{
		return nullptr;
	}

	return CreateRenderTarget( pDevice, pTexture->Get( ), rtvDesc, renderTargetName );
}

IDepthStencil* CRenderTargetManager::CreateDepthStencil( ID3D11Device* pDevice, const ITexture* pTexture, const D3D11_DEPTH_STENCIL_VIEW_DESC* dsvDesc, const String& depthStencilName )
{
	if ( auto depthStencil = FindDepthStencil( depthStencilName ) )
	{
		return depthStencil;
	}

	if ( pDevice && pTexture )
	{
		std::unique_ptr<CDepthStencil> newDepthStencil = std::make_unique<CDepthStencil>();

		if ( newDepthStencil->CreateDepthStencil( pDevice, pTexture, dsvDesc ) )
		{
			CDepthStencil* ret = newDepthStencil.get( );
			RegisterDepthStencil( depthStencilName, std::move( newDepthStencil ) );
			return ret;
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

IRenderTarget* CRenderTargetManager::CreateRenderTarget( ID3D11Device* pDevice, ID3D11Resource* pResource, const D3D11_RENDER_TARGET_VIEW_DESC* rtvDesc, const String& renderTargetName )
{
	if ( auto renderTarget = FindRenderTarget( renderTargetName ) )
	{
		return renderTarget;
	}

	if ( pDevice && pResource )
	{
		std::unique_ptr<CRenderTarget> newRenderTarget = std::make_unique<CRenderTarget>();

		if ( newRenderTarget->CreateRenderTarget( pDevice, pResource, rtvDesc ) )
		{
			CRenderTarget* ret = newRenderTarget.get( );
			RegisterRenderTarget( renderTargetName, std::move( newRenderTarget ) );
			return ret;
		}
	}

	return nullptr;
}

void CRenderTargetManager::RegisterRenderTarget( const String& renderTargetName, std::unique_ptr<IRenderTarget> renderTarget )
{
	m_renderTargets.emplace( renderTargetName, std::move( renderTarget ) );
}

void CRenderTargetManager::RegisterDepthStencil( const String& depthStencilName, std::unique_ptr<IDepthStencil> depthStencil )
{
	m_depthStencils.emplace( depthStencilName, std::move( depthStencil ) );
}