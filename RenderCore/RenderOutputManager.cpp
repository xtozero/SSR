#include "stdafx.h"
#include "RenderOutputManager.h"

#include "CommonRenderer/IRenderer.h"
#include "CommonRenderer/IRenderResource.h"
#include "CommonRenderer/IRenderResourceManager.h"

#include "../Shared/Util.h"

#include <cassert>
#include <DXGI.h>
#include <D3D11.h>
#include <wrl\client.h>

bool CRenderOutputManager::Initialize( IResourceManager& resourceMgr, IDXGISwapChain& pSwapChain )
{
	ON_FAIL_RETURN( CreateDefaultRenderTaraget( resourceMgr, pSwapChain ) );
	ON_FAIL_RETURN( CreateDefaultDepthStencil( resourceMgr ) );
	ON_FAIL_RETURN( CreateNormalRenderTarget( resourceMgr ) );
	ON_FAIL_RETURN( CreateDepthRenderTarget( resourceMgr ) );

	return true;
}

void CRenderOutputManager::AppSizeChanged( IResourceManager& resourceMgr, IDXGISwapChain& pSwapChain )
{
	Initialize( resourceMgr, pSwapChain );
}

void CRenderOutputManager::SetRenderTargetDepthStencilView( IRenderer& renderer )
{
	RenderTargetBinder binder;
	binder.Bind( 0, m_renderOutputs[FRAME_BUFFER] ? static_cast<ID3D11RenderTargetView*>( m_renderOutputs[FRAME_BUFFER]->Get( ) ) : nullptr );
	binder.Bind( 1, m_renderOutputs[NORMAL_BUFFER] ? static_cast<ID3D11RenderTargetView*>( m_renderOutputs[NORMAL_BUFFER]->Get( ) ) : nullptr );
	binder.Bind( 2, m_renderOutputs[DEPTH_BUFFER] ? static_cast<ID3D11RenderTargetView*>( m_renderOutputs[DEPTH_BUFFER]->Get( ) ) : nullptr );

	renderer.SetRenderTarget( binder, m_pPrimeDs );
}

void CRenderOutputManager::ClearDepthStencil( IRenderer& renderer )
{
	renderer.ClearDepthStencil( *m_pPrimeDs, 1.0f, 0 );
}

void CRenderOutputManager::ClearRenderTargets( IRenderer& renderer, const rtClearColor& clearColor )
{
	float color[4] = { std::get<0>( clearColor ),
						std::get<1>( clearColor ),
						std::get<2>( clearColor ),
						std::get<3>( clearColor ) };

	for ( int i = 0; i < COUNT; ++i )
	{
		renderer.ClearRendertarget( *m_renderOutputs[i], color );
	}
}

void CRenderOutputManager::SceneEnd( ID3D11DeviceContext& deviceContext )
{
	if ( m_renderSRVs[DEPTH_BUFFER] )
	{
		deviceContext.GenerateMips( static_cast<ID3D11ShaderResourceView*>( m_renderSRVs[DEPTH_BUFFER]->Get( ) ) );
	}
}

CRenderOutputManager::CRenderOutputManager( ) :
	m_pPrimeDs( nullptr )
{
	m_renderOutputs.fill( nullptr );
	m_renderSRVs.fill( nullptr );
}

bool CRenderOutputManager::CreateDefaultRenderTaraget( IResourceManager& resourceMgr, IDXGISwapChain& pSwapChain )
{
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pd3d11BackBuffer = nullptr;
	if ( SUCCEEDED( pSwapChain.GetBuffer( 0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>( pd3d11BackBuffer.GetAddressOf() ) ) ) )
	{
		String renderTargetTexName( _T( "DefaultRenderTarget" ) );

		ITexture* pBackBufferTex = resourceMgr.RegisterTexture2D( renderTargetTexName, pd3d11BackBuffer.Get(), true );
		if ( pBackBufferTex == nullptr )
		{
			return false;
		}

		m_renderOutputs[FRAME_BUFFER] = resourceMgr.CreateRenderTarget( *pBackBufferTex, renderTargetTexName );
		if ( m_renderOutputs[FRAME_BUFFER] == nullptr )
		{
			return false;
		}

		String duplicateTexName( _T( "DuplicateFrameBuffer" ) );
		ITexture* pDuplicateTex = resourceMgr.CreateCloneTexture( *pBackBufferTex, duplicateTexName );

		if ( pDuplicateTex == nullptr )
		{
			return false;
		}

		m_renderSRVs[FRAME_BUFFER] = resourceMgr.FindShaderResource( duplicateTexName );
	}

	return true;
}

bool CRenderOutputManager::CreateNormalRenderTarget( IResourceManager& resourceMgr )
{
	String normalTexName( _T( "NormalGBuffer" ) );
	ITexture* pGBufferNormal = resourceMgr.CreateTexture2D( normalTexName, normalTexName );
	if ( pGBufferNormal == nullptr )
	{
		return false;
	}

	m_renderOutputs[NORMAL_BUFFER] = resourceMgr.CreateRenderTarget( *pGBufferNormal, normalTexName );

	if ( m_renderOutputs[NORMAL_BUFFER] == nullptr )
	{
		return false;
	}

	m_renderSRVs[NORMAL_BUFFER] = resourceMgr.CreateShaderResource( *pGBufferNormal, normalTexName );

	return true;
}

bool CRenderOutputManager::CreateDepthRenderTarget( IResourceManager& resourceMgr )
{
	String depthTexName( _T( "DepthGBuffer" ) );
	ITexture* pGBufferDepth = resourceMgr.CreateTexture2D( depthTexName, depthTexName );
	if ( pGBufferDepth == nullptr )
	{
		return false;
	}

	m_renderOutputs[DEPTH_BUFFER] = resourceMgr.CreateRenderTarget( *pGBufferDepth, depthTexName );
	if ( m_renderOutputs[DEPTH_BUFFER] == nullptr )
	{
		return false;
	}

	String duplicateTexName( _T( "DuplicateDepthGBuffer" ) );
	ITexture* pDuplicateTex = resourceMgr.CreateCloneTexture( *pGBufferDepth, duplicateTexName );

	if ( pDuplicateTex == nullptr )
	{
		return false;
	}

	m_renderSRVs[DEPTH_BUFFER] = resourceMgr.FindShaderResource( duplicateTexName );

	return true;
}

bool CRenderOutputManager::CreateDefaultDepthStencil( IResourceManager& resourceMgr )
{
	String depthStencilTexName( _T( "DefaultDepthStencil" ) );
	const ITexture* depthStencilTexture = resourceMgr.CreateTexture2D( depthStencilTexName, depthStencilTexName );

	if ( depthStencilTexture == nullptr )
	{
		return false;
	}

	TEXTURE_TRAIT trait = depthStencilTexture->GetTrait();
	trait.m_format = RESOURCE_FORMAT::D24_UNORM_S8_UINT;

	m_pPrimeDs = resourceMgr.CreateDepthStencil( *depthStencilTexture, depthStencilTexName, &trait );

	if ( m_pPrimeDs == nullptr )
	{
		return false;
	}

	return true;
}