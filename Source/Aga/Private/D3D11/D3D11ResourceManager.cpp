#include "stdafx.h"
#include "D3D11ResourceManager.h"

#include "Buffer.h"

#include "common.h"

#include "D3D11BaseTexture.h"
#include "D3D11BlendState.h"
#include "D3D11Buffer.h"
#include "D3D11DepthStencilState.h"
#include "D3D11FlagConvertor.h"
#include "D3D11PipelineState.h"
#include "D3D11RasterizerState.h"
#include "D3D11SamplerState.h"
#include "D3D11Shaders.h"
#include "D3D11VetexLayout.h"
#include "D3D11Viewport.h"

#include "DataStructure/EnumStringMap.h"

#include "Texture.h"

#include "Util.h"

#include <cstddef>
#include <fstream>

namespace aga
{
	IResourceManager* g_resourceManager = nullptr;

	void CD3D11ResourceManager::AppSizeChanged( UINT nWndWidth, UINT nWndHeight )
	{
		m_frameBufferSize.first = nWndWidth;
		m_frameBufferSize.second = nWndHeight;
	}

	Texture* CD3D11ResourceManager::CreateTexture( const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData )
	{
		if ( trait.m_miscFlag & RESOURCE_MISC::APP_SIZE_DEPENDENT )
		{
			// TODO : 텍스쳐 리사이즈 로직필요
			//trait.m_width = m_frameBufferSize.first;
			//trait.m_height = m_frameBufferSize.second;
		}

		Texture* newTexture = nullptr;
		if ( IsTexture1D( trait ) )
		{
			newTexture = new D3D11BaseTexture1D( trait, initData );
		}
		else if ( IsTexture2D( trait ) )
		{
			newTexture = new D3D11BaseTexture2D( trait, initData );
		}
		else if ( IsTexture3D( trait ) )
		{
			newTexture = new D3D11BaseTexture3D( trait, initData );
		}
		else
		{
			return nullptr;
		}

		m_renderResources.emplace( newTexture );
		return newTexture;
	}

	Buffer* CD3D11ResourceManager::CreateBuffer( const BUFFER_TRAIT& trait, const void* initData )
	{
		Buffer* newBuffer = new D3D11Buffer( trait, initData );

		return newBuffer;
	}

	VertexLayout* CD3D11ResourceManager::CreateVertexLayout( const VertexShader* vs, const VERTEX_LAYOUT_TRAIT* trait, std::size_t size )
	{
		auto d3d11VS = static_cast<const D3D11VertexShader*>( vs );
		auto newVertexLayout = new D3D11VertexLayout( d3d11VS, trait, size );

		return newVertexLayout;
	}

	ComputeShader * CD3D11ResourceManager::CreateComputeShader( const void* byteCode, std::size_t byteCodeSize )
	{
		auto newShader = new D3D11ComputeShader( byteCode, byteCodeSize );
		m_renderResources.emplace( newShader );

		return newShader;
	}

	VertexShader* CD3D11ResourceManager::CreateVertexShader( const void* byteCode, std::size_t byteCodeSize )
	{
		auto newShader = new D3D11VertexShader( byteCode, byteCodeSize );
		m_renderResources.emplace( newShader );

		return newShader;
	}

	PixelShader* CD3D11ResourceManager::CreatePixelShader( const void* byteCode, std::size_t byteCodeSize )
	{
		auto newShader = new D3D11PixelShader( byteCode, byteCodeSize );
		m_renderResources.emplace( newShader );

		return newShader;
	}

	BlendState* CD3D11ResourceManager::CreateBlendState( const BLEND_STATE_TRAIT& trait )
	{
		auto blendState = new D3D11BlendState( trait );
		m_renderResources.emplace( blendState );

		return blendState;
	}

	DepthStencilState* CD3D11ResourceManager::CreateDepthStencilState( const DEPTH_STENCIL_STATE_TRAIT& trait )
	{
		auto depthStencilState = new D3D11DepthStencilState( trait );
		m_renderResources.emplace( depthStencilState );

		return depthStencilState;
	}

	RasterizerState* CD3D11ResourceManager::CreateRasterizerState( const RASTERIZER_STATE_TRAIT& trait )
	{
		auto rasterizerState = new D3D11RasterizerState( trait );
		m_renderResources.emplace( rasterizerState );

		return rasterizerState;
	}

	SamplerState* CD3D11ResourceManager::CreateSamplerState( const SAMPLER_STATE_TRAIT& trait )
	{
		auto samplerState = new D3D11SamplerState( trait );
		m_renderResources.emplace( samplerState );

		return samplerState;
	}

	PipelineState* CD3D11ResourceManager::CreatePipelineState( const PipelineStateInitializer& initializer )
	{
		auto cached = m_pipelineStateCache.find( initializer );
		if ( cached != m_pipelineStateCache.end( ) )
		{
			return cached->second;
		}

		auto pipelineState = new D3D11PipelineState( initializer );
		m_pipelineStateCache.emplace( initializer, pipelineState );

		return pipelineState;
	}

	Viewport* CD3D11ResourceManager::CreateViewport( int width, int height, void* hWnd, RESOURCE_FORMAT format )
	{
		auto viewport = new D3D11Viewport( width, height, hWnd, ConvertFormatToDxgiFormat( format ) );
		m_renderResources.emplace( viewport );

		return viewport;
	}

	//void CD3D11ResourceManager::CopyResource( RE_HANDLE dest, const RESOURCE_REGION* destRegionOrNull, RE_HANDLE src, const RESOURCE_REGION* srcRegionOrNull )
	//{
	//	ID3D11Resource* pDest = nullptr/*GetD3D11ResourceGeneric( dest )*/;
	//	ID3D11Resource* pSrc = nullptr /*GetD3D11ResourceGeneric( src )*/;
	//
	//	assert( pDest != nullptr && pSrc != nullptr );
	//
	//	if ( destRegionOrNull == nullptr || srcRegionOrNull == nullptr )
	//	{
	//		m_pDeviceContext->CopyResource( pDest, pSrc );
	//	}
	//	else
	//	{
	//		const RESOURCE_REGION& destRegion = *destRegionOrNull;
	//		const RESOURCE_REGION& srcRegion = *srcRegionOrNull;
	//
	//		D3D11_BOX box = { srcRegion.m_left,  srcRegion.m_top,  srcRegion.m_front,  srcRegion.m_right,  srcRegion.m_bottom,  srcRegion.m_back };
	//
	//		m_pDeviceContext->CopySubresourceRegion( pDest, destRegion.m_subResource, destRegion.m_left, destRegion.m_top, destRegion.m_front, pSrc, srcRegion.m_subResource, &box );
	//	}
	//}

	//void CD3D11ResourceManager::UpdateResourceFromMemory( RE_HANDLE dest, void* src, UINT srcRowPitch, UINT srcDepthPitch, const RESOURCE_REGION* destRegionOrNull )
	//{
	//	ID3D11Resource* pDest = nullptr;/*GetD3D11ResourceGeneric( dest )*/;
	//
	//	D3D11_BOX destBox = {};
	//	UINT destSubresouce = 0;
	//	if ( destRegionOrNull != nullptr )
	//	{
	//		destBox = { destRegionOrNull->m_left, destRegionOrNull->m_top, destRegionOrNull->m_front, destRegionOrNull->m_right, destRegionOrNull->m_bottom, destRegionOrNull->m_back };
	//		destSubresouce = destRegionOrNull->m_subResource;
	//	}
	//
	//	m_pDeviceContext->UpdateSubresource( pDest, destSubresouce, destRegionOrNull ? &destBox : nullptr, src, srcRowPitch, srcDepthPitch );
	//}

	void CreateD3D11ResourceManager( )
	{
		g_resourceManager = new CD3D11ResourceManager( );
	}

	void DestoryD3D11ResourceManager( )
	{
		delete g_resourceManager;
	}

	void* GetD3D11ResourceManager( )
	{
		return g_resourceManager;
	}
}