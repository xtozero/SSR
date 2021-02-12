#include "stdafx.h"
#include "D3D11ResourceManager.h"

#include "Buffer.h"

#include "common.h"

#include "D3D11BaseTexture.h"
#include "D3D11BlendState.h"
#include "D3D11Buffer.h"
#include "D3D11DepthStencil.h"
#include "D3D11DepthStencilState.h"
#include "D3D11RandomAccessResource.h"
#include "D3D11RasterizerState.h"
#include "D3D11RenderTarget.h"
#include "D3D11SamplerState.h"
#include "D3D11ShaderResource.h"
#include "D3D11Shaders.h"
#include "D3D11VetexLayout.h"
#include "D3D11Viewport.h"

#include "DataStructure/EnumStringMap.h"

#include "Texture.h"

#include "Util.h"

#include <cstddef>
#include <DXGI.h>
#include <fstream>

bool CD3D11ResourceManager::Bootup( ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext )
{
	OnDeviceRestore( pDevice, pDeviceContext );
	return true;
}

void CD3D11ResourceManager::AppSizeChanged( UINT nWndWidth, UINT nWndHeight )
{
	m_frameBufferSize.first = nWndWidth;
	m_frameBufferSize.second = nWndHeight;
}

aga::Texture* CD3D11ResourceManager::CreateTexture( const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData )
{
	if ( trait.m_miscFlag & RESOURCE_MISC::APP_SIZE_DEPENDENT )
	{
		// TODO : 텍스쳐 리사이즈 로직필요
		//trait.m_width = m_frameBufferSize.first;
		//trait.m_height = m_frameBufferSize.second;
	}

	aga::Texture* newTexture = nullptr;
	if ( aga::IsTexture1D( trait ) )
	{
		newTexture = new aga::D3D11BaseTexture1D( trait, initData );
	}
	else if ( aga::IsTexture2D( trait ) )
	{
		newTexture = new aga::D3D11BaseTexture2D( trait, initData );
	}
	else if ( aga::IsTexture3D( trait ) )
	{
		newTexture = new aga::D3D11BaseTexture3D( trait, initData );
	}
	else
	{
		return nullptr;
	}

	m_renderResources.emplace( newTexture );
	return newTexture;
}

aga::Buffer* CD3D11ResourceManager::CreateBuffer( const BUFFER_TRAIT& trait, const RESOURCE_INIT_DATA* initData )
{
	aga::Buffer* newBuffer = new D3D11BufferBase( trait, initData );
	m_renderResources.emplace( newBuffer );

	return newBuffer;
}

VertexLayout* CD3D11ResourceManager::FindAndCreateVertexLayout( const aga::VertexShader* vs, const VertexLayoutDesc& layoutDesc )
{
	auto found = m_vertexLayouts.find( layoutDesc );
	if ( found != m_vertexLayouts.end( ) )
	{
		return found->second.Get();
	}

	auto newLayout = new D3D11VertexLayout( reinterpret_cast<const D3D11VertexShader*>( vs ), layoutDesc );
	m_vertexLayouts.emplace( layoutDesc, newLayout );

	return newLayout;
}

aga::VertexShader* CD3D11ResourceManager::CreateVertexShader( const void* byteCode, std::size_t byteCodeSize )
{
	auto newShader = new D3D11VertexShader( byteCode, byteCodeSize );
	m_renderResources.emplace( newShader );

	return newShader;
}

aga::PixelShader* CD3D11ResourceManager::CreatePixelShader( const void* byteCode, std::size_t byteCodeSize )
{
	auto newShader = new D3D11PixelShader( byteCode, byteCodeSize );
	m_renderResources.emplace( newShader );

	return newShader;
}

RE_HANDLE CD3D11ResourceManager::CreateSamplerState( const SAMPLER_STATE_TRAIT& trait )
{
	auto resource = new CD3D11SamplerState( trait );

	auto found = std::find( m_samplerStates.begin( ), m_samplerStates.end( ), nullptr );
	if ( found != m_samplerStates.end( ) )
	{
		*found = resource;
	}
	else
	{
		m_samplerStates.emplace_back( resource );
	}

	return RE_HANDLE( GraphicsResourceType::SAMPLER_STATE, resource );
}

RE_HANDLE CD3D11ResourceManager::CreateRasterizerState( const RASTERIZER_STATE_TRAIT& trait )
{
	auto resource = new CD3D11RasterizerState( trait );

	auto found = std::find( m_rasterizerStates.begin( ), m_rasterizerStates.end( ), nullptr );
	if ( found != m_rasterizerStates.end( ) )
	{
		*found = resource;
	}
	else
	{
		m_rasterizerStates.emplace_back( resource );
	}

	return RE_HANDLE( GraphicsResourceType::RASTERIZER_STATE, resource );
}

RE_HANDLE CD3D11ResourceManager::CreateBlendState( const BLEND_STATE_TRAIT& trait )
{
	auto resource = new CD3D11BlendState( trait );

	auto found = std::find( m_blendStates.begin( ), m_blendStates.end( ), nullptr );
	if ( found != m_blendStates.end( ) )
	{
		*found = resource;
	}
	else
	{
		m_blendStates.emplace_back( resource );
	}

	return RE_HANDLE( GraphicsResourceType::BLEND_STATE, resource );
}

RE_HANDLE CD3D11ResourceManager::CreateDepthStencilState( const DEPTH_STENCIL_STATE_TRAIT& trait )
{
	auto resource = new CD3D11DepthStencilState( trait );

	auto found = std::find( m_depthStencilStates.begin( ), m_depthStencilStates.end( ), nullptr );
	if ( found != m_depthStencilStates.end( ) )
	{
		*found = resource;
	}
	else
	{
		m_depthStencilStates.emplace_back( resource );
	}

	return RE_HANDLE( GraphicsResourceType::DEPTH_STENCIL_STATE, resource );
}

aga::Viewport* CD3D11ResourceManager::CreateViewport( int width, int height, void* hWnd, DXGI_FORMAT format )
{
	auto viewport = new aga::D3D11Viewport( width, height, hWnd, format );
	m_renderResources.emplace( viewport );

	return viewport;
}

void CD3D11ResourceManager::CopyResource( RE_HANDLE dest, const RESOURCE_REGION* destRegionOrNull, RE_HANDLE src, const RESOURCE_REGION* srcRegionOrNull )
{
	ID3D11Resource* pDest = nullptr/*GetD3D11ResourceGeneric( dest )*/;
	ID3D11Resource* pSrc = nullptr /*GetD3D11ResourceGeneric( src )*/;

	assert( pDest != nullptr && pSrc != nullptr );

	if ( destRegionOrNull == nullptr || srcRegionOrNull == nullptr )
	{
		m_pDeviceContext->CopyResource( pDest, pSrc );
	}
	else
	{
		const RESOURCE_REGION& destRegion = *destRegionOrNull;
		const RESOURCE_REGION& srcRegion = *srcRegionOrNull;

		D3D11_BOX box = { srcRegion.m_left,  srcRegion.m_top,  srcRegion.m_front,  srcRegion.m_right,  srcRegion.m_bottom,  srcRegion.m_back };

		m_pDeviceContext->CopySubresourceRegion( pDest, destRegion.m_subResource, destRegion.m_left, destRegion.m_top, destRegion.m_front, pSrc, srcRegion.m_subResource, &box );
	}
}

void CD3D11ResourceManager::UpdateResourceFromMemory( RE_HANDLE dest, void* src, UINT srcRowPitch, UINT srcDepthPitch, const RESOURCE_REGION* destRegionOrNull )
{
	ID3D11Resource* pDest = nullptr;/*GetD3D11ResourceGeneric( dest )*/;

	D3D11_BOX destBox = {};
	UINT destSubresouce = 0;
	if ( destRegionOrNull != nullptr )
	{
		destBox = { destRegionOrNull->m_left, destRegionOrNull->m_top, destRegionOrNull->m_front, destRegionOrNull->m_right, destRegionOrNull->m_bottom, destRegionOrNull->m_back };
		destSubresouce = destRegionOrNull->m_subResource;
	}

	m_pDeviceContext->UpdateSubresource( pDest, destSubresouce, destRegionOrNull ? &destBox : nullptr, src, srcRowPitch, srcDepthPitch );
}

void CD3D11ResourceManager::FreeResource( RE_HANDLE handle )
{
	if ( handle.IsValid( ) == false )
	{
		return;
	}

	IDeviceDependant* graphicsResource = GetGraphicsResource( handle );
	if ( graphicsResource )
	{
		graphicsResource->Free( );
	}
}

void CD3D11ResourceManager::OnDeviceLost( )
{
	for ( auto samplerState : m_samplerStates )
	{
		samplerState->Free( );
	}

	for ( auto rasterizerState : m_rasterizerStates )
	{
		rasterizerState->Free( );
	}

	for ( auto blendState : m_blendStates )
	{
		blendState->Free( );
	}

	for ( auto depthStencilState : m_depthStencilStates )
	{
		depthStencilState->Free( );
	}
}

void CD3D11ResourceManager::OnDeviceRestore( ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext )
{
	m_pDevice = pDevice;
	m_pDeviceContext = pDeviceContext;
}

CD3D11SamplerState* CD3D11ResourceManager::GetSamplerState( RE_HANDLE handle ) const
{
	assert( IsSamplerStateHandle( handle ) );
	return reinterpret_cast<CD3D11SamplerState*>( handle.m_resource.Get( ) );
}

CD3D11RasterizerState* CD3D11ResourceManager::GetRasterizerState( RE_HANDLE handle ) const
{
	assert( IsRasterizerStateHandle( handle ) );
	return reinterpret_cast<CD3D11RasterizerState*>( handle.m_resource.Get( ) );
}

CD3D11BlendState* CD3D11ResourceManager::GetBlendState( RE_HANDLE handle ) const
{
	assert( IsBlendStateHandle( handle ) );
	return reinterpret_cast<CD3D11BlendState*>( handle.m_resource.Get( ) );
}

CD3D11DepthStencilState* CD3D11ResourceManager::GetDepthStencilState( RE_HANDLE handle ) const
{
	assert( IsDepthStencilStateHandle( handle ) );
	return reinterpret_cast<CD3D11DepthStencilState*>( handle.m_resource.Get( ) );
}

IDeviceDependant* CD3D11ResourceManager::GetGraphicsResource( RE_HANDLE handle ) const
{
	GraphicsResourceType resourceType = handle.m_type;

	switch ( resourceType )
	{
	case GraphicsResourceType::SAMPLER_STATE:
		{
			return reinterpret_cast<CD3D11SamplerState*>( handle.m_resource.Get( ) );
		}
		break;
	case GraphicsResourceType::RASTERIZER_STATE:
		{
			return reinterpret_cast<CD3D11RasterizerState*>( handle.m_resource.Get( ) );
		}
		break;
	case GraphicsResourceType::BLEND_STATE:
		{
			return reinterpret_cast<CD3D11BlendState*>( handle.m_resource.Get( ) );
		}
		break;
	case GraphicsResourceType::DEPTH_STENCIL_STATE:
		{
			return reinterpret_cast<CD3D11DepthStencilState*>( handle.m_resource.Get( ) );
		}
		break;
	default:
		assert( false && "invalid resource handle" );
		break;
	}

	return nullptr;
}
