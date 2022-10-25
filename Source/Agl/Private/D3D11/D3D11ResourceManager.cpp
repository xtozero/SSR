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

#include "EnumStringMap.h"

#include "Texture.h"

#include <cstddef>
#include <fstream>

namespace agl
{
	void CD3D11ResourceManager::Shutdown()
	{
		m_pipelineStateCache.clear();
	}

	Texture* CD3D11ResourceManager::CreateTexture( const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData )
	{
		if ( HasAnyFlags( trait.m_miscFlag, ResourceMisc::AppSizeDependent ) )
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

		return newTexture;
	}

	Buffer* CD3D11ResourceManager::CreateBuffer( const BUFFER_TRAIT& trait, const void* initData )
	{
		Buffer* newBuffer = new D3D11Buffer( trait, initData );

		return newBuffer;
	}

	VertexLayout* CD3D11ResourceManager::CreateVertexLayout( const VertexShader* vs, const VERTEX_LAYOUT_TRAIT* trait, uint32 size )
	{
		auto d3d11VS = static_cast<const D3D11VertexShader*>( vs );
		auto newVertexLayout = new D3D11VertexLayout( d3d11VS, trait, size );

		return newVertexLayout;
	}

	ComputeShader * CD3D11ResourceManager::CreateComputeShader( const void* byteCode, size_t byteCodeSize )
	{
		auto newShader = new D3D11ComputeShader( byteCode, byteCodeSize );

		return newShader;
	}

	VertexShader* CD3D11ResourceManager::CreateVertexShader( const void* byteCode, size_t byteCodeSize )
	{
		auto newShader = new D3D11VertexShader( byteCode, byteCodeSize );

		return newShader;
	}

	GeometryShader* CD3D11ResourceManager::CreateGeometryShader( const void* byteCode, size_t byteCodeSize )
	{
		auto newShader = new D3D11GeometryShader( byteCode, byteCodeSize );

		return newShader;
	}

	PixelShader* CD3D11ResourceManager::CreatePixelShader( const void* byteCode, size_t byteCodeSize )
	{
		auto newShader = new D3D11PixelShader( byteCode, byteCodeSize );

		return newShader;
	}

	BlendState* CD3D11ResourceManager::CreateBlendState( const BLEND_STATE_TRAIT& trait )
	{
		auto blendState = new D3D11BlendState( trait );

		return blendState;
	}

	DepthStencilState* CD3D11ResourceManager::CreateDepthStencilState( const DEPTH_STENCIL_STATE_TRAIT& trait )
	{
		auto depthStencilState = new D3D11DepthStencilState( trait );

		return depthStencilState;
	}

	RasterizerState* CD3D11ResourceManager::CreateRasterizerState( const RASTERIZER_STATE_TRAIT& trait )
	{
		auto rasterizerState = new D3D11RasterizerState( trait );

		return rasterizerState;
	}

	SamplerState* CD3D11ResourceManager::CreateSamplerState( const SAMPLER_STATE_TRAIT& trait )
	{
		auto samplerState = new D3D11SamplerState( trait );

		return samplerState;
	}

	PipelineState* CD3D11ResourceManager::CreatePipelineState( const PipelineStateInitializer& initializer )
	{
		auto cached = m_pipelineStateCache.find( initializer );
		if ( cached != m_pipelineStateCache.end() )
		{
			return cached->second;
		}

		auto pipelineState = new D3D11PipelineState( initializer );
		m_pipelineStateCache.emplace( initializer, pipelineState );

		return pipelineState;
	}

	Viewport* CD3D11ResourceManager::CreateViewport( uint32 width, uint32 height, void* hWnd, ResourceFormat format )
	{
		auto viewport = new D3D11Viewport( width, height, hWnd, ConvertFormatToDxgiFormat( format ) );

		return viewport;
	}

	CD3D11ResourceManager::~CD3D11ResourceManager()
	{
		Shutdown();
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

	//void CD3D11ResourceManager::UpdateResourceFromMemory( RE_HANDLE dest, void* src, uint32 srcRowPitch, uint32 srcDepthPitch, const RESOURCE_REGION* destRegionOrNull )
	//{
	//	ID3D11Resource* pDest = nullptr;/*GetD3D11ResourceGeneric( dest )*/;
	//
	//	D3D11_BOX destBox = {};
	//	uint32 destSubresouce = 0;
	//	if ( destRegionOrNull != nullptr )
	//	{
	//		destBox = { destRegionOrNull->m_left, destRegionOrNull->m_top, destRegionOrNull->m_front, destRegionOrNull->m_right, destRegionOrNull->m_bottom, destRegionOrNull->m_back };
	//		destSubresouce = destRegionOrNull->m_subResource;
	//	}
	//
	//	m_pDeviceContext->UpdateSubresource( pDest, destSubresouce, destRegionOrNull ? &destBox : nullptr, src, srcRowPitch, srcDepthPitch );
	//}

	Owner<IResourceManager*> CreateD3D11ResourceManager()
	{
		return new CD3D11ResourceManager();
	}
}