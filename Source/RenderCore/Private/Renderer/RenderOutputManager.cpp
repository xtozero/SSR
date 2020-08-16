#include "stdafx.h"
/* #include "Render/RenderOutputManager.h"

#include "Render/IRenderer.h"
#include "Render/IRenderResourceManager.h"
#include "Util.h"

#include <cassert>
#include <DXGI.h>
#include <D3D11.h>
#include <wrl\client.h>

bool CRenderOutputManager::Initialize( IResourceManager& resourceMgr, IDXGISwapChain& pSwapChain, DXGI_SAMPLE_DESC& sampleDesc )
{
	if ( CreateDefaultRenderTaraget( resourceMgr, pSwapChain ) == false )
	{
		__debugbreak( );
	}
	
	if ( CreateDefaultDepthStencil( resourceMgr, sampleDesc ) == false )
	{
		__debugbreak( );
	}

	if ( CreateNormalRenderTarget( resourceMgr, sampleDesc ) == false )
	{
		__debugbreak( );
	}

	if ( CreateDepthRenderTarget( resourceMgr, sampleDesc ) == false )
	{
		__debugbreak( );
	}

	return true;
}

void CRenderOutputManager::AppSizeChanged( IResourceManager& resourceMgr, IDXGISwapChain& pSwapChain, DXGI_SAMPLE_DESC& sampleDesc )
{
	Initialize( resourceMgr, pSwapChain, sampleDesc );
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

CRenderOutputManager::CRenderOutputManager( )
{
	m_renderOutputs.fill( RE_HANDLE::InValidHandle( ) );
	m_renderSRVs.fill( RE_HANDLE::InValidHandle( ) );
}

bool CRenderOutputManager::CreateDefaultRenderTaraget( IResourceManager& resourceMgr, IDXGISwapChain& pSwapChain )
{
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pd3d11BackBuffer = nullptr;
	if ( SUCCEEDED( pSwapChain.GetBuffer( 0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>( pd3d11BackBuffer.GetAddressOf() ) ) ) )
	{
		std::string renderTargetTexName( "DefaultRenderTarget" );

		RE_HANDLE hBackBufferTex = resourceMgr.RegisterTexture2D( renderTargetTexName, pd3d11BackBuffer.Get(), true );
		if ( hBackBufferTex == RE_HANDLE::InValidHandle( ) )
		{
			return false;
		}

		m_renderOutputs[FRAME_BUFFER] = resourceMgr.CreateRenderTarget( hBackBufferTex, renderTargetTexName );
		if ( m_renderOutputs[FRAME_BUFFER] == RE_HANDLE::InValidHandle( ) )
		{
			return false;
		}

		std::string duplicateTexName( "DuplicateFrameBuffer" );
		RE_HANDLE hDuplicateTex = resourceMgr.CreateCloneTexture( hBackBufferTex, duplicateTexName );

		if ( hDuplicateTex == RE_HANDLE::InValidHandle( ) )
		{
			return false;
		}

		// Fix....
		m_renderSRVs[FRAME_BUFFER] = resourceMgr.FindShaderResource( duplicateTexName );
	}

	return true;
}

bool CRenderOutputManager::CreateNormalRenderTarget( IResourceManager& resourceMgr, DXGI_SAMPLE_DESC& sampleDesc )
{
	std::string normalTexName( "NormalGBuffer" );

	TEXTURE_TRAIT normalGBufferTrait = {
		0U,
		0U,
		1U,
		sampleDesc.Count,
		sampleDesc.Quality,
		1U,
		RESOURCE_FORMAT::R16G16_FLOAT,
		RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::GPU_WRITE,
		RESOURCE_BIND_TYPE::RENDER_TARGET | RESOURCE_BIND_TYPE::SHADER_RESOURCE,
		RESOURCE_MISC::APP_SIZE_DEPENDENT
	};

	RE_HANDLE hGBufferNormal = resourceMgr.CreateTexture2D( normalGBufferTrait, normalTexName );
	if ( hGBufferNormal == RE_HANDLE::InValidHandle( ) )
	{
		return false;
	}

	m_renderOutputs[NORMAL_BUFFER] = resourceMgr.CreateRenderTarget( hGBufferNormal, normalTexName );

	if ( m_renderOutputs[NORMAL_BUFFER] == RE_HANDLE::InValidHandle( ) )
	{
		return false;
	}

	m_renderSRVs[NORMAL_BUFFER] = resourceMgr.CreateTextureShaderResource( hGBufferNormal, normalTexName );

	return true;
}

bool CRenderOutputManager::CreateDepthRenderTarget( IResourceManager& resourceMgr, DXGI_SAMPLE_DESC& sampleDesc )
{
	std::string depthTexName( "DepthGBuffer" );

	TEXTURE_TRAIT depthGBufferTrait = {
		0U,
		0U,
		1U,
		sampleDesc.Count,
		sampleDesc.Quality,
		1U,
		RESOURCE_FORMAT::R32_FLOAT,
		RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::GPU_WRITE,
		RESOURCE_BIND_TYPE::RENDER_TARGET | RESOURCE_BIND_TYPE::SHADER_RESOURCE,
		RESOURCE_MISC::APP_SIZE_DEPENDENT | RESOURCE_MISC::GENERATE_MIPS
	};

	RE_HANDLE hGBufferDepth = resourceMgr.CreateTexture2D( depthGBufferTrait, depthTexName );
	if ( hGBufferDepth == RE_HANDLE::InValidHandle( ) )
	{
		return false;
	}

	m_renderOutputs[DEPTH_BUFFER] = resourceMgr.CreateRenderTarget( hGBufferDepth, depthTexName );
	if ( m_renderOutputs[DEPTH_BUFFER] == RE_HANDLE::InValidHandle( ) )
	{
		return false;
	}

	std::string duplicateTexName( "DuplicateDepthGBuffer" );
	RE_HANDLE hDuplicateTex = resourceMgr.CreateCloneTexture( hGBufferDepth, duplicateTexName );

	if ( hDuplicateTex == RE_HANDLE::InValidHandle( ) )
	{
		return false;
	}

	m_renderSRVs[DEPTH_BUFFER] = resourceMgr.FindShaderResource( duplicateTexName );

	return true;
}

bool CRenderOutputManager::CreateDefaultDepthStencil( IResourceManager& resourceMgr, DXGI_SAMPLE_DESC& sampleDesc )
{
	std::string depthStencilTexName( "DefaultDepthStencil" );
	
	TEXTURE_TRAIT depthStencilTrait = {
		0U,
		0U,
		1U,
		sampleDesc.Count,
		sampleDesc.Quality,
		1U,
		RESOURCE_FORMAT::R24G8_TYPELESS,
		RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::GPU_WRITE,
		RESOURCE_BIND_TYPE::DEPTH_STENCIL | RESOURCE_BIND_TYPE::SHADER_RESOURCE,
		RESOURCE_MISC::APP_SIZE_DEPENDENT
	};

	RE_HANDLE hDepthStencilTexture = resourceMgr.CreateTexture2D( depthStencilTrait, depthStencilTexName );

	if ( hDepthStencilTexture == RE_HANDLE::InValidHandle( ) )
	{
		return false;
	}

	TEXTURE_TRAIT trait = resourceMgr.GetTextureTrait( hDepthStencilTexture );
	trait.m_format = RESOURCE_FORMAT::D24_UNORM_S8_UINT;

	m_primeDs = resourceMgr.CreateDepthStencil( hDepthStencilTexture, depthStencilTexName, &trait );

	if ( m_primeDs == RE_HANDLE::InValidHandle( ) )
	{
		return false;
	}

	return true;
}
*/