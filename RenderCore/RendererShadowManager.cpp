#include "stdafx.h"
#include "RendererShadowManager.h"

#include "IDepthStencil.h"
#include "ITexture.h"
#include "IShaderResource.h"
#include "IRenderTarget.h"

#include "IRenderer.h"
#include "RenderTargetManager.h"
#include "ShaderResourceManager.h"
#include "TextureManager.h"

#include <D3D11.h>

std::unique_ptr<IRendererShadowManager> CreateShadowManager()
{
	return std::make_unique<CRendererShadowManager>();
}

CRendererShadowManager::CRendererShadowManager() :
	m_shadowMap( nullptr ),
	m_srvShadowMap( nullptr ),
	m_rtvShadowMap( nullptr ),
	m_dsvShadowMap( nullptr ),
	m_isEnabled( false )
{
}

void CRendererShadowManager::SceneBegin( IRenderer* pRenderer )
{
	if ( pRenderer == nullptr )
	{
		return;
	}

	if ( m_isEnabled == false )
	{
		return;
	}

	ID3D11DeviceContext* pDeviceContext = pRenderer->GetDeviceContext( );
	CRenderTargetManager* pRenderTargetMgr = pRenderer->GetRenderTargetManager( );

	if ( pDeviceContext == nullptr || pRenderTargetMgr == nullptr )
	{
		return;
	}

	ID3D11ShaderResourceView* srv[1] = { nullptr };
	pDeviceContext->PSSetShaderResources( 2, 1, srv );
	pRenderTargetMgr->SetRenderTarget( pDeviceContext, m_rtvShadowMap, m_dsvShadowMap );
	
	constexpr float clearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	
	m_rtvShadowMap->Clear( pDeviceContext, clearColor );
	m_dsvShadowMap->Clear( pDeviceContext, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0 );
}

void CRendererShadowManager::SceneEnd( IRenderer* pRenderer )
{
	if ( pRenderer == nullptr )
	{
		return;
	}

	if ( m_isEnabled == false )
	{
		return;
	}
	
	ID3D11DeviceContext* pDeviceContext = pRenderer->GetDeviceContext( );
	CRenderTargetManager* pRenderTargetMgr = pRenderer->GetRenderTargetManager( );

	if ( pDeviceContext == nullptr || pRenderTargetMgr == nullptr )
	{
		return;
	}
	
	pRenderTargetMgr->SetRenderTarget( pDeviceContext, nullptr, nullptr );
	ID3D11ShaderResourceView* srv = m_srvShadowMap->Get( );
	pDeviceContext->PSSetShaderResources( 2, 1, &srv );
}

void CRendererShadowManager::CreateShadowMapTexture( IRenderer* pRenderer )
{
	m_isEnabled = false;

	if ( pRenderer == nullptr )
	{
		return;
	}

	ID3D11Device* pDevice = pRenderer->GetDevice( );
	CTextureManager* pTextureMgr = pRenderer->GetTextureManager( );
	if ( pDevice == nullptr || pTextureMgr == nullptr )
	{
		return;
	}

	m_shadowMap = pTextureMgr->CreateTexture2D( pDevice, _T( "ShadowMap" ), _T( "ShadowMap" ) );
	if ( m_shadowMap == nullptr )
	{
		return;
	}

	CRenderTargetManager* pRenderTargetMgr = pRenderer->GetRenderTargetManager( );
	if ( pRenderTargetMgr == nullptr )
	{
		return;
	}

	m_rtvShadowMap = pRenderTargetMgr->CreateRenderTarget( pDevice, m_shadowMap->Get( ), nullptr, _T( "ShadowMap" ) );
	if ( m_rtvShadowMap == nullptr )
	{
		return;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC desc;
	::ZeroMemory( &desc, sizeof( desc ) );

	desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	desc.Texture2D.MipLevels = 1;
	desc.Format = DXGI_FORMAT_R32_FLOAT;

	CShaderResourceManager* pShaderResourceMgr = pRenderer->GetShaderResourceManager( );
	if ( pShaderResourceMgr == nullptr )
	{
		return;
	}

	m_srvShadowMap = pShaderResourceMgr->CreateShaderResource( pDevice, m_shadowMap, desc, _T( "ShadowMap" ) );
	if ( m_srvShadowMap == nullptr )
	{
		return;
	}

	const ITexture* depthStencilTexture = pTextureMgr->CreateTexture2D( pDevice, _T("ShadowMapDepthStencil"), _T( "ShadowMapDepthStencil" ) );
	if ( depthStencilTexture == nullptr )
	{
		return;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC d3d11DSDesc;
	::ZeroMemory( &d3d11DSDesc, sizeof( d3d11DSDesc ) );

	d3d11DSDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	d3d11DSDesc.Texture2D.MipSlice = 0;
	d3d11DSDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	m_dsvShadowMap = pRenderTargetMgr->CreateDepthStencil( pDevice, depthStencilTexture, &d3d11DSDesc, _T( "ShadowMapDepthStencil" ) );
	if ( m_dsvShadowMap == nullptr )
	{
		return;
	}

	m_isEnabled = true;
}
