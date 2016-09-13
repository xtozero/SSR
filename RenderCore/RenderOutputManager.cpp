#include "stdafx.h"
#include "RenderOutputManager.h"

#include "IDepthStencil.h"
#include "IRenderer.h"
#include "IRenderResourceManager.h"
#include "IRenderTarget.h"
#include "IShaderResource.h"
#include "ITexture.h"
#include "TextureDescription.h"

#include "../Shared/Util.h"

#include <cassert>
#include <DXGI.h>
#include <D3D11.h>
#include <wrl\client.h>

bool CRenderOutputManager::Initialize( IRenderer& renderer )
{
	ON_FAIL_RETURN( CreateDefaultRenderTaraget( renderer ) );
	ON_FAIL_RETURN( CreateDefaultDepthStencil( renderer ) );
	ON_FAIL_RETURN( CreateNormalRenderTarget( renderer ) );
	ON_FAIL_RETURN( CreateDepthRenderTarget( renderer ) );

	return true;
}

void CRenderOutputManager::SetRenderTargetDepthStencilView( IRenderer& renderer )
{
	IRenderTargetManager& renderTargetMgr = renderer.GetRenderTargetManager( );

	ID3D11DeviceContext* pDeviceContext = renderer.GetDeviceContext( );
	RenderTargetBinder binder;
	binder.Bind( 0, m_renderOutputs[FRAME_BUFFER] ? m_renderOutputs[FRAME_BUFFER]->Get( ) : nullptr );
	binder.Bind( 1, m_renderOutputs[NORMAL_BUFFER] ? m_renderOutputs[NORMAL_BUFFER]->Get( ) : nullptr );
	binder.Bind( 2, m_renderOutputs[DEPTH_BUFFER] ? m_renderOutputs[DEPTH_BUFFER]->Get( ) : nullptr );

	renderTargetMgr.SetRenderTarget( pDeviceContext, binder, m_pPrimeDs );
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

void CRenderOutputManager::SceneEnd( ID3D11DeviceContext* pDeviceContext )
{
	if ( pDeviceContext && m_renderSRVs[DEPTH_BUFFER] )
	{
		pDeviceContext->GenerateMips( m_renderSRVs[DEPTH_BUFFER]->Get( ) );
	}
}

CRenderOutputManager::CRenderOutputManager( ) :
	m_pPrimeDs( nullptr )
{
	m_renderOutputs.fill( nullptr );
	m_renderSRVs.fill( nullptr );
}

bool CRenderOutputManager::CreateDefaultRenderTaraget( IRenderer& renderer )
{
	ID3D11Device* pDevice = renderer.GetDevice( );
	IDXGISwapChain* pSwapChain = renderer.GetSwapChain( );
	ITextureManager& textureMgr = renderer.GetTextureManager( );
	IRenderTargetManager& renderTargetMgr = renderer.GetRenderTargetManager( );
	IShaderResourceManager& shaderResourceMgr = renderer.GetShaderResourceManager( );
	ISnapshotManager& snapshotMgr = renderer.GetSnapshotManager( );

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

		ITexture* pBackBufferTex = textureMgr.FindTexture( renderTargetTexName );
		m_renderOutputs[FRAME_BUFFER] = renderTargetMgr.CreateRenderTarget( pDevice, pBackBufferTex, nullptr, renderTargetTexName );
		if ( m_renderOutputs[FRAME_BUFFER] == nullptr )
		{
			return false;
		}

		String duplicateTexName( _T( "DuplicateFrameBuffer" ) );
		ITexture* pDuplicateTex = snapshotMgr.CreateCloneTexture( pDevice, pBackBufferTex, duplicateTexName );

		if ( pDuplicateTex == nullptr )
		{
			return false;
		}

		m_renderSRVs[FRAME_BUFFER] = shaderResourceMgr.FindShaderResource( duplicateTexName );
	}

	return true;
}

bool CRenderOutputManager::CreateNormalRenderTarget( IRenderer& renderer )
{
	ID3D11Device* pDevice = renderer.GetDevice( );
	ITextureManager& textureMgr = renderer.GetTextureManager( );
	IRenderTargetManager& renderTargetMgr = renderer.GetRenderTargetManager( );
	IShaderResourceManager& shaderResourceMgr = renderer.GetShaderResourceManager( );

	if ( pDevice == nullptr )
	{
		return false;
	}

	String normalTexName( _T( "NormalGBuffer" ) );
	ITexture* pGBufferNormal = textureMgr.CreateTexture2D( pDevice, normalTexName, normalTexName );
	m_renderOutputs[NORMAL_BUFFER] = renderTargetMgr.CreateRenderTarget( pDevice, pGBufferNormal, nullptr, normalTexName );

	if ( m_renderOutputs[NORMAL_BUFFER] == nullptr )
	{
		return false;
	}

	m_renderSRVs[NORMAL_BUFFER] = shaderResourceMgr.CreateShaderResource( pDevice, pGBufferNormal, nullptr, normalTexName );

	return true;
}

bool CRenderOutputManager::CreateDepthRenderTarget( IRenderer& renderer )
{
	ID3D11Device* pDevice = renderer.GetDevice( );
	ITextureManager& textureMgr = renderer.GetTextureManager( );
	IRenderTargetManager& renderTargetMgr = renderer.GetRenderTargetManager( );
	IShaderResourceManager& shaderResourceMgr = renderer.GetShaderResourceManager( );
	ISnapshotManager& snapshotMgr = renderer.GetSnapshotManager( );

	if ( pDevice == nullptr )
	{
		return false;
	}

	String depthTexName( _T( "DepthGBuffer" ) );
	ITexture* pGBufferDepth = textureMgr.CreateTexture2D( pDevice, depthTexName, depthTexName );
	m_renderOutputs[DEPTH_BUFFER] = renderTargetMgr.CreateRenderTarget( pDevice, pGBufferDepth, nullptr, depthTexName );
	if ( m_renderOutputs[DEPTH_BUFFER] == nullptr )
	{
		return false;
	}

	String duplicateTexName( _T( "DuplicateDepthGBuffer" ) );
	ITexture* pDuplicateTex = snapshotMgr.CreateCloneTexture( pDevice, pGBufferDepth, duplicateTexName );

	if ( pDuplicateTex == nullptr )
	{
		return false;
	}

	m_renderSRVs[DEPTH_BUFFER] = shaderResourceMgr.FindShaderResource( duplicateTexName );

	return true;
}

bool CRenderOutputManager::CreateDefaultDepthStencil( IRenderer& renderer )
{
	ID3D11Device* pDevice = renderer.GetDevice( );
	ITextureManager& textureMgr = renderer.GetTextureManager( );
	IRenderTargetManager& renderTargetMgr = renderer.GetRenderTargetManager( );

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