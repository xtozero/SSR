#include "stdafx.h"
#include "RenderOutputManager.h"

#include "CommonRenderer/IRenderer.h"
#include "CommonRenderer/IRenderResourceManager.h"

#include "../Shared/Util.h"

#include <cassert>
#include <DXGI.h>
#include <D3D11.h>
#include <wrl\client.h>

using namespace RE_HANDLE_TYPE;

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
	renderer.BindRenderTargets( m_renderOutputs.data(), 3, m_primeDs );
}

void CRenderOutputManager::ClearDepthStencil( IRenderer& renderer )
{
	renderer.ClearDepthStencil( m_primeDs, 1.0f, 0 );
}

void CRenderOutputManager::ClearRenderTargets( IRenderer& renderer, const float (&clearColor)[4] )
{
	for ( int i = 0; i < COUNT; ++i )
	{
		renderer.ClearRendertarget( m_renderOutputs[i], clearColor );
	}
}

void CRenderOutputManager::SceneEnd( IRenderer& renderer )
{
	if ( m_renderSRVs[DEPTH_BUFFER] )
	{
		renderer.GenerateMips( m_renderSRVs[DEPTH_BUFFER] );
	}
}

CRenderOutputManager::CRenderOutputManager( )
{
	m_renderOutputs.fill( INVALID_HANDLE );
	m_renderSRVs.fill( INVALID_HANDLE );
}

bool CRenderOutputManager::CreateDefaultRenderTaraget( IResourceManager& resourceMgr, IDXGISwapChain& pSwapChain )
{
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pd3d11BackBuffer = nullptr;
	if ( SUCCEEDED( pSwapChain.GetBuffer( 0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>( pd3d11BackBuffer.GetAddressOf() ) ) ) )
	{
		String renderTargetTexName( _T( "DefaultRenderTarget" ) );

		RE_HANDLE hBackBufferTex = resourceMgr.RegisterTexture2D( renderTargetTexName, pd3d11BackBuffer.Get(), true );
		if ( hBackBufferTex == INVALID_HANDLE )
		{
			return false;
		}

		m_renderOutputs[FRAME_BUFFER] = resourceMgr.CreateRenderTarget( hBackBufferTex, renderTargetTexName );
		if ( m_renderOutputs[FRAME_BUFFER] == INVALID_HANDLE )
		{
			return false;
		}

		String duplicateTexName( _T( "DuplicateFrameBuffer" ) );
		RE_HANDLE hDuplicateTex = resourceMgr.CreateCloneTexture( hBackBufferTex, duplicateTexName );

		if ( hDuplicateTex == INVALID_HANDLE )
		{
			return false;
		}

		// Fix....
		m_renderSRVs[FRAME_BUFFER] = resourceMgr.FindShaderResource( duplicateTexName );
	}

	return true;
}

bool CRenderOutputManager::CreateNormalRenderTarget( IResourceManager& resourceMgr )
{
	String normalTexName( _T( "NormalGBuffer" ) );
	RE_HANDLE hGBufferNormal = resourceMgr.CreateTexture2D( normalTexName, normalTexName );
	if ( hGBufferNormal == INVALID_HANDLE )
	{
		return false;
	}

	m_renderOutputs[NORMAL_BUFFER] = resourceMgr.CreateRenderTarget( hGBufferNormal, normalTexName );

	if ( m_renderOutputs[NORMAL_BUFFER] == INVALID_HANDLE )
	{
		return false;
	}

	m_renderSRVs[NORMAL_BUFFER] = resourceMgr.CreateTextureShaderResource( hGBufferNormal, normalTexName );

	return true;
}

bool CRenderOutputManager::CreateDepthRenderTarget( IResourceManager& resourceMgr )
{
	String depthTexName( _T( "DepthGBuffer" ) );
	RE_HANDLE hGBufferDepth = resourceMgr.CreateTexture2D( depthTexName, depthTexName );
	if ( hGBufferDepth == INVALID_HANDLE )
	{
		return false;
	}

	m_renderOutputs[DEPTH_BUFFER] = resourceMgr.CreateRenderTarget( hGBufferDepth, depthTexName );
	if ( m_renderOutputs[DEPTH_BUFFER] == INVALID_HANDLE )
	{
		return false;
	}

	String duplicateTexName( _T( "DuplicateDepthGBuffer" ) );
	RE_HANDLE hDuplicateTex = resourceMgr.CreateCloneTexture( hGBufferDepth, duplicateTexName );

	if ( hDuplicateTex == INVALID_HANDLE )
	{
		return false;
	}

	m_renderSRVs[DEPTH_BUFFER] = resourceMgr.FindShaderResource( duplicateTexName );

	return true;
}

bool CRenderOutputManager::CreateDefaultDepthStencil( IResourceManager& resourceMgr )
{
	String depthStencilTexName( _T( "DefaultDepthStencil" ) );
	RE_HANDLE hDepthStencilTexture = resourceMgr.CreateTexture2D( depthStencilTexName, depthStencilTexName );

	if ( hDepthStencilTexture == INVALID_HANDLE )
	{
		return false;
	}

	TEXTURE_TRAIT trait = resourceMgr.GetTextureTrait( hDepthStencilTexture );
	trait.m_format = RESOURCE_FORMAT::D24_UNORM_S8_UINT;

	m_primeDs = resourceMgr.CreateDepthStencil( hDepthStencilTexture, depthStencilTexName, &trait );

	if ( m_primeDs == INVALID_HANDLE )
	{
		return false;
	}

	return true;
}