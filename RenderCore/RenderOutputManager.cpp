#include "stdafx.h"
#include "RenderOutputManager.h"

#include "IDepthStencil.h"
#include "IRenderer.h"
#include "IRenderTarget.h"
#include "RenderTargetManager.h"
#include "TextureManager.h"

#include <cassert>
#include <DXGI.h>
#include <D3D11.h>
#include <wrl\client.h>

bool CRenderOutputManager::Initialize( IRenderer* pRenderer )
{
	if ( pRenderer )
	{
		ID3D11Device* pDevice = pRenderer->GetDevice( );
		IDXGISwapChain* pDxgiSwapChain = pRenderer->GetSwapChain( );
		CTextureManager* pTextureMgr = pRenderer->GetTextureManager( );
		CRenderTargetManager* pRernderTargetMgr = pRenderer->GetRenderTargetManager( );

		if ( pTextureMgr == nullptr || pRernderTargetMgr == nullptr )
		{
			assert( pTextureMgr && pRernderTargetMgr );
			return false;
		}

		ON_FAIL_RETURN( CreateDefaultRenderTaraget( pDevice, pDxgiSwapChain, *pTextureMgr, *pRernderTargetMgr ) );
		ON_FAIL_RETURN( CreateDefaultDepthStencil( pDevice, *pTextureMgr, *pRernderTargetMgr ) );
		ON_FAIL_RETURN( CreateNormalRenderTarget( pDevice, *pTextureMgr, *pRernderTargetMgr ) );
		
		return true;
	}

	return false;
}

void CRenderOutputManager::SetRenderTargetDepthStencilView( IRenderer* pRenderer )
{
	if ( pRenderer == nullptr )
	{
		assert( pRenderer );
		return;
	}

	CRenderTargetManager* rtManager = pRenderer->GetRenderTargetManager( );

	if ( rtManager == nullptr )
	{
		assert( rtManager );
		return;
	}

	ID3D11DeviceContext* pDeviceContext = pRenderer->GetDeviceContext( );
	RenderTargetBinder binder;
	binder.Bind( 0, m_renderOutputs[FRAME_BUFFER] ? m_renderOutputs[FRAME_BUFFER]->Get( ) : nullptr );
	binder.Bind( 1, m_renderOutputs[NORMAL_BUFFER] ? m_renderOutputs[NORMAL_BUFFER]->Get( ) : nullptr );

	rtManager->SetRenderTarget( pDeviceContext, binder, m_pPrimeDs );
}

void CRenderOutputManager::ClearDepthStencil( ID3D11DeviceContext* pDeviceContext )
{
	if ( m_pPrimeDs )
	{
		m_pPrimeDs->Clear( pDeviceContext, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0 );
	}
}

void CRenderOutputManager::ClearRenderTargets( ID3D11DeviceContext* pDeviceContext, const rtClearColor& clearColor )
{
	if ( pDeviceContext )
	{
		float color[4] = { std::get<0>( clearColor ),
							std::get<1>( clearColor ),
							std::get<2>( clearColor ),
							std::get<3>( clearColor ) };

		for ( int i = 0; i < COUNT; ++i )
		{
			if ( m_renderOutputs[i] )
			{
				m_renderOutputs[i]->Clear( pDeviceContext, color );
			}
		}
	}
}

CRenderOutputManager::CRenderOutputManager( ) :
	m_pPrimeDs( nullptr )
{
	m_renderOutputs.fill( nullptr );
}


CRenderOutputManager::~CRenderOutputManager( )
{
}

bool CRenderOutputManager::CreateDefaultRenderTaraget( ID3D11Device * pDevice, IDXGISwapChain* pSwapChain, CTextureManager & textureMgr, CRenderTargetManager & renderTargetMgr )
{
	if ( pDevice == nullptr || pSwapChain == nullptr )
	{
		return false;
	}

	Microsoft::WRL::ComPtr<ID3D11Texture2D> pd3d11BackBuffer;

	if ( SUCCEEDED( pSwapChain->GetBuffer( 0, __uuidof(ID3D11Texture2D), (LPVOID*)&pd3d11BackBuffer ) ) )
	{
		Microsoft::WRL::ComPtr<ID3D11Resource> renderTargetResource;
		String renderTargetTexName( _T( "DefaultRenderTarget" ) );

		if ( SUCCEEDED( pd3d11BackBuffer.Get( )->QueryInterface( IID_ID3D11Resource, &renderTargetResource ) ) )
		{
			ON_FAIL_RETURN( textureMgr.RegisterTexture2D( renderTargetTexName, renderTargetResource ) );
		}

		m_renderOutputs[FRAME_BUFFER] = renderTargetMgr.CreateRenderTarget( pDevice, pd3d11BackBuffer.Get( ), nullptr, renderTargetTexName );
		if ( m_renderOutputs[FRAME_BUFFER] == nullptr )
		{
			return false;
		}
	}

	return true;
}

bool CRenderOutputManager::CreateNormalRenderTarget( ID3D11Device * pDevice, CTextureManager & textureMgr, CRenderTargetManager & renderTargetMgr )
{
	if ( pDevice == nullptr )
	{
		return false;
	}

	String normalTexName( _T( "NormalGBuffer" ) );
	ITexture* pGBufferNormal = textureMgr.CreateTexture2D( pDevice, normalTexName, normalTexName );

	if ( pGBufferNormal == nullptr )
	{
		return false;
	}

	m_renderOutputs[NORMAL_BUFFER] = renderTargetMgr.CreateRenderTarget( pDevice, pGBufferNormal->Get( ), nullptr, normalTexName );

	if ( m_renderOutputs[NORMAL_BUFFER] == nullptr )
	{
		return false;
	}

	return true;
}

bool CRenderOutputManager::CreateDefaultDepthStencil( ID3D11Device* pDevice,  CTextureManager & textureMgr, CRenderTargetManager & renderTargetMgr )
{
	if ( pDevice == nullptr )
	{
		return false;
	}

	String depthStencilTexName( _T( "DefaultDepthStencil" ) );
	const ITexture* depthStencilTexture = textureMgr.CreateTexture2D( pDevice, depthStencilTexName, depthStencilTexName );

	if ( depthStencilTexture == nullptr )
	{
		return false;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC d3d11DSDesc;
	::ZeroMemory( &d3d11DSDesc, sizeof( d3d11DSDesc ) );

	d3d11DSDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	d3d11DSDesc.Texture2D.MipSlice = 0;
	d3d11DSDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	m_pPrimeDs = renderTargetMgr.CreateDepthStencil( pDevice, depthStencilTexture, &d3d11DSDesc, depthStencilTexName );

	if ( m_pPrimeDs == nullptr )
	{
		return false;
	}

	return true;
}