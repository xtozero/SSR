#include "stdafx.h"
#include "D3D11ResourceManager.h"

#include "common.h"

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
#include "D3D11Textures.h"
#include "D3D11VetexLayout.h"

#include "DataStructure/EnumStringMap.h"
#include "DataStructure/KeyValueReader.h"
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

	for ( auto depthStencil : m_depthStencils )
	{
		if ( depthStencil->IsAppSizeDependency( ) )
		{
			depthStencil->Free( );
		}
	}

	for ( auto renderTarget : m_renderTargets )
	{
		if ( renderTarget->IsAppSizeDependency( ) )
		{
			renderTarget->Free( );
		}
	}

	for ( auto shaderResource : m_shaderResources )
	{
		if ( shaderResource->IsAppSizeDependency( ) )
		{
			shaderResource->Free( );
		}
	}

	for ( auto randomAccessResource : m_randomAccessResources )
	{
		if ( randomAccessResource->IsAppSizeDependency( ) )
		{
			randomAccessResource->Free( );
		}
	}

	// 리사이즈 필요
	for ( auto texture : m_texture2Ds )
	{
		if ( texture->IsAppSizeDependency( ) )
		{
			texture->Free( );
		}
	}
}

RE_HANDLE CD3D11ResourceManager::CreateTexture1D( TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData )
{
	if ( trait.m_miscFlag & RESOURCE_MISC::APP_SIZE_DEPENDENT )
	{
		trait.m_width = m_frameBufferSize.first;
		trait.m_height = m_frameBufferSize.second;
	}

	auto resource = new CD3D11Texture1D( trait, initData );

	auto found = std::find( m_texture1Ds.begin( ), m_texture1Ds.end( ), nullptr );
	if ( found != m_texture1Ds.end( ) )
	{
		*found = resource;
	}
	else
	{
		m_texture1Ds.emplace_back( resource );
	}

	return RE_HANDLE( GraphicsResourceType::TEXTURE1D, resource );
}

RE_HANDLE CD3D11ResourceManager::CreateTexture2D( TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData )
{
	if ( trait.m_miscFlag & RESOURCE_MISC::APP_SIZE_DEPENDENT )
	{
		trait.m_width = m_frameBufferSize.first;
		trait.m_height = m_frameBufferSize.second;
	}

	auto resource = new CD3D11Texture2D( trait, initData );

	auto found = std::find( m_texture2Ds.begin( ), m_texture2Ds.end( ), nullptr );
	if ( found != m_texture2Ds.end( ) )
	{
		*found = resource;
	}
	else
	{
		m_texture2Ds.emplace_back( resource );
	}

	return RE_HANDLE( GraphicsResourceType::TEXTURE2D, resource );
}

RE_HANDLE CD3D11ResourceManager::CreateTexture3D( TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA * initData )
{
	if ( trait.m_miscFlag & RESOURCE_MISC::APP_SIZE_DEPENDENT )
	{
		trait.m_width = m_frameBufferSize.first;
		trait.m_height = m_frameBufferSize.second;
	}

	auto resource = new CD3D11Texture3D( trait, initData );

	auto found = std::find( m_texture3Ds.begin( ), m_texture3Ds.end( ), nullptr );
	if ( found != m_texture3Ds.end( ) )
	{
		*found = resource;
	}
	else
	{
		m_texture3Ds.emplace_back( resource );
	}

	return RE_HANDLE( GraphicsResourceType::TEXTURE3D, resource );
}

RE_HANDLE CD3D11ResourceManager::CreateBuffer( const BUFFER_TRAIT& trait, const RESOURCE_INIT_DATA* initData )
{
	auto resource = new CD3D11Buffer( trait, initData );

	auto found = std::find( m_buffers.begin( ), m_buffers.end( ), nullptr );
	if ( found != m_buffers.end( ) )
	{
		*found = resource;
	}
	else
	{
		m_buffers.emplace_back( resource );
	}

	return RE_HANDLE( GraphicsResourceType::BUFFER, resource );
}

RE_HANDLE CD3D11ResourceManager::CreateVertexLayout( RE_HANDLE vsHandle, const VERTEX_LAYOUT* layoutOrNull, int layoutSize )
{
	CD3D11VertexShader* vs = GetVertexShader( vsHandle );

	auto resource = new CD3D11VertexLayout( vs, layoutOrNull, layoutSize );

	auto found = std::find( m_vertexLayouts.begin( ), m_vertexLayouts.end( ), nullptr );
	if ( found != m_vertexLayouts.end( ) )
	{
		*found = resource;
	}
	else
	{
		m_vertexLayouts.emplace_back( resource );
	}

	return RE_HANDLE( GraphicsResourceType::VERTEX_LAYOUT, resource );
}

RE_HANDLE CD3D11ResourceManager::CreateVertexShader( const void* byteCodePtr, std::size_t byteCodeSize )
{
	auto resource = new CD3D11VertexShader( byteCodePtr, byteCodeSize );

	auto found = std::find( m_vertexShaders.begin( ), m_vertexShaders.end( ), nullptr );
	if ( found != m_vertexShaders.end( ) )
	{
		*found = resource;
	}
	else
	{
		m_vertexShaders.emplace_back( resource );
	}

	return RE_HANDLE( GraphicsResourceType::VERTEX_SHADER, resource );
}

RE_HANDLE CD3D11ResourceManager::CreateGeometryShader( const void* byteCodePtr, std::size_t byteCodeSize )
{
	auto resource = new CD3D11GeometryShader( byteCodePtr, byteCodeSize );

	auto found = std::find( m_geometryShaders.begin( ), m_geometryShaders.end( ), nullptr );
	if ( found != m_geometryShaders.end( ) )
	{
		*found = resource;
	}
	else
	{
		m_geometryShaders.emplace_back( resource );
	}

	return RE_HANDLE( GraphicsResourceType::GEOMETRY_SHADER, resource );
}

RE_HANDLE CD3D11ResourceManager::CreatePixelShader( const void* byteCodePtr, std::size_t byteCodeSize )
{
	auto resource = new CD3D11PixelShader( byteCodePtr, byteCodeSize );

	auto found = std::find( m_pixelShaders.begin( ), m_pixelShaders.end( ), nullptr );
	if ( found != m_pixelShaders.end( ) )
	{
		*found = resource;
	}
	else
	{
		m_pixelShaders.emplace_back( resource );
	}

	return RE_HANDLE( GraphicsResourceType::PIXEL_SHADER, resource );
}

RE_HANDLE CD3D11ResourceManager::CreateComputeShader( const void* byteCodePtr, std::size_t byteCodeSize )
{
	auto resource = new CD3D11ComputeShader( byteCodePtr, byteCodeSize );

	auto found = std::find( m_computeShaders.begin( ), m_computeShaders.end( ), nullptr );
	if ( found != m_computeShaders.end( ) )
	{
		*found = resource;
	}
	else
	{
		m_computeShaders.emplace_back( resource );
	}

	return RE_HANDLE( GraphicsResourceType::COMPUTE_SHADER, resource );
}

RE_HANDLE CD3D11ResourceManager::CreateRenderTarget( RE_HANDLE texHandle, const TEXTURE_TRAIT* trait )
{
	assert( texHandle.IsValid( ) );
	assert( IsTexture2DHandle( texHandle ) );

	CD3D11Texture2D* texture = GetTexture2D( texHandle );
	auto resource = new CD3D11RenderTarget( texture, trait );

	auto found = std::find( m_renderTargets.begin( ), m_renderTargets.end( ), nullptr );
	if ( found != m_renderTargets.end( ) )
	{
		*found = resource;
	}
	else
	{
		m_renderTargets.emplace_back( resource );
	}

	return RE_HANDLE( GraphicsResourceType::RENDER_TARGET, resource );
}

RE_HANDLE CD3D11ResourceManager::CreateDepthStencil( RE_HANDLE texHandle, const TEXTURE_TRAIT* trait )
{
	assert( texHandle.IsValid( ) );
	assert( IsTexture2DHandle( texHandle ) );

	CD3D11Texture2D* texture = GetTexture2D( texHandle );
	auto resource = new CD3D11DepthStencil( texture, trait );

	auto found = std::find( m_depthStencils.begin( ), m_depthStencils.end( ), nullptr );
	if ( found != m_depthStencils.end( ) )
	{
		*found = resource;
	}
	else
	{
		m_depthStencils.emplace_back( resource );
	}

	return RE_HANDLE( GraphicsResourceType::DEPTH_STENCIL, resource );
}

RE_HANDLE CD3D11ResourceManager::CreateTexture1DShaderResource( RE_HANDLE texHandle, const TEXTURE_TRAIT* trait )
{
	assert( texHandle.IsValid( ) );
	assert( IsTexture1DHandle( texHandle ) );

	CD3D11Texture1D* texture = GetTexture1D( texHandle );
	auto resource = new CD3D11ShaderResource( texture, trait );

	auto found = std::find( m_shaderResources.begin( ), m_shaderResources.end( ), nullptr );
	if ( found != m_shaderResources.end( ) )
	{
		*found = resource;
	}
	else
	{
		m_shaderResources.emplace_back( resource );
	}

	return RE_HANDLE( GraphicsResourceType::SHADER_RESOURCE, resource );
}

RE_HANDLE CD3D11ResourceManager::CreateTexture2DShaderResource( RE_HANDLE texHandle, const TEXTURE_TRAIT* trait )
{
	assert( texHandle.IsValid( ) );
	assert( IsTexture1DHandle( texHandle ) );

	CD3D11Texture2D* texture = GetTexture2D( texHandle );
	auto resource = new CD3D11ShaderResource( texture, trait );

	auto found = std::find( m_shaderResources.begin( ), m_shaderResources.end( ), nullptr );
	if ( found != m_shaderResources.end( ) )
	{
		*found = resource;
	}
	else
	{
		m_shaderResources.emplace_back( resource );
	}

	return RE_HANDLE( GraphicsResourceType::SHADER_RESOURCE, resource );
}

RE_HANDLE CD3D11ResourceManager::CreateTexture3DShaderResource( RE_HANDLE texHandle, const TEXTURE_TRAIT* trait )
{
	assert( texHandle.IsValid( ) );
	assert( IsTexture1DHandle( texHandle ) );

	CD3D11Texture3D* texture = GetTexture3D( texHandle );
	auto resource = new CD3D11ShaderResource( texture, trait );

	auto found = std::find( m_shaderResources.begin( ), m_shaderResources.end( ), nullptr );
	if ( found != m_shaderResources.end( ) )
	{
		*found = resource;
	}
	else
	{
		m_shaderResources.emplace_back( resource );
	}

	return RE_HANDLE( GraphicsResourceType::SHADER_RESOURCE, resource );
}

RE_HANDLE CD3D11ResourceManager::CreateBufferShaderResource( RE_HANDLE bufHandle, const BUFFER_TRAIT* trait )
{
	assert( bufHandle.IsValid( ) );
	assert( IsBufferHandle( bufHandle ) );

	CD3D11Buffer* buffer = GetBuffer( bufHandle );
	auto resource = new CD3D11ShaderResource( buffer, trait );

	auto found = std::find( m_shaderResources.begin( ), m_shaderResources.end( ), nullptr );
	if ( found != m_shaderResources.end( ) )
	{
		*found = resource;
	}
	else
	{
		m_shaderResources.emplace_back( resource );
	}

	return RE_HANDLE( GraphicsResourceType::SHADER_RESOURCE, resource );
}

RE_HANDLE CD3D11ResourceManager::CreateTexture1DRandomAccess( RE_HANDLE texHandle, const TEXTURE_TRAIT* trait )
{
	assert( texHandle.IsValid( ) );
	assert( IsTexture1DHandle( texHandle ) );

	CD3D11Texture1D* texture = GetTexture1D( texHandle );
	auto resource = new CD3D11RandomAccessResource( texture, trait );

	auto found = std::find( m_randomAccessResources.begin( ), m_randomAccessResources.end( ), nullptr );
	if ( found != m_randomAccessResources.end( ) )
	{
		*found = resource;
	}
	else
	{
		m_randomAccessResources.emplace_back( resource );
	}

	return RE_HANDLE( GraphicsResourceType::RANDOM_ACCESS, resource );
}

RE_HANDLE CD3D11ResourceManager::CreateTexture2DRandomAccess( RE_HANDLE texHandle, const TEXTURE_TRAIT* trait )
{
	assert( texHandle.IsValid( ) );
	assert( IsTexture2DHandle( texHandle ) );

	CD3D11Texture2D* texture = GetTexture2D( texHandle );
	auto resource = new CD3D11RandomAccessResource( texture, trait );

	auto found = std::find( m_randomAccessResources.begin( ), m_randomAccessResources.end( ), nullptr );
	if ( found != m_randomAccessResources.end( ) )
	{
		*found = resource;
	}
	else
	{
		m_randomAccessResources.emplace_back( resource );
	}

	return RE_HANDLE( GraphicsResourceType::RANDOM_ACCESS, resource );
}

RE_HANDLE CD3D11ResourceManager::CreateBufferRandomAccess( RE_HANDLE bufHandle, const BUFFER_TRAIT* trait )
{
	assert( bufHandle.IsValid( ) );
	assert( IsBufferHandle( bufHandle ) );

	CD3D11Buffer* buffer = GetBuffer( bufHandle );
	auto resource = new CD3D11RandomAccessResource( buffer, trait );

	auto found = std::find( m_randomAccessResources.begin( ), m_randomAccessResources.end( ), nullptr );
	if ( found != m_randomAccessResources.end( ) )
	{
		*found = resource;
	}
	else
	{
		m_randomAccessResources.emplace_back( resource );
	}

	return RE_HANDLE( GraphicsResourceType::RANDOM_ACCESS, resource );
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

void CD3D11ResourceManager::CopyResource( RE_HANDLE dest, const RESOURCE_REGION* destRegionOrNull, RE_HANDLE src, const RESOURCE_REGION* srcRegionOrNull )
{
	ID3D11Resource* pDest = GetD3D11ResourceGeneric( dest );
	ID3D11Resource* pSrc = GetD3D11ResourceGeneric( src );

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
	ID3D11Resource* pDest = GetD3D11ResourceGeneric( dest );

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
	for ( auto depthStencil : m_depthStencils )
	{
		depthStencil->Free( );
	}

	for ( auto renderTarget : m_renderTargets )
	{
		renderTarget->Free( );
	}

	for ( auto shaderResource : m_shaderResources )
	{
		shaderResource->Free( );
	}

	for ( auto randomAccessResource : m_randomAccessResources )
	{
		randomAccessResource->Free( );
	}

	for ( auto texture : m_texture1Ds )
	{
		texture->Free( );
	}

	for ( auto texture : m_texture2Ds )
	{
		texture->Free( );
	}

	for ( auto texture : m_texture3Ds )
	{
		texture->Free( );
	}

	for ( auto buffer : m_buffers )
	{
		buffer->Free( );
	}

	m_vertexShaders.clear( );
	m_geometryShaders.clear( );
	m_pixelShaders.clear( );
	m_computeShaders.clear( );

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

CD3D11DepthStencil* CD3D11ResourceManager::GetDepthstencil( RE_HANDLE handle ) const
{
	assert( IsDepthStencilHandle( handle ) );
	return reinterpret_cast<CD3D11DepthStencil*>( handle.m_resource.Get( ) );
}

CD3D11RenderTarget* CD3D11ResourceManager::GetRendertarget( RE_HANDLE handle ) const
{
	assert( IsRenderTargetHandle( handle ) );
	return reinterpret_cast<CD3D11RenderTarget*>( handle.m_resource.Get( ) );
}

CD3D11ShaderResource* CD3D11ResourceManager::GetShaderResource( RE_HANDLE handle ) const
{
	assert( IsShaderResourceHandle( handle ) );
	return reinterpret_cast<CD3D11ShaderResource*>( handle.m_resource.Get( ) );
}

CD3D11RandomAccessResource* CD3D11ResourceManager::GetRandomAccess( RE_HANDLE handle ) const
{
	assert( IsRandomAccessHandle( handle ) );
	return reinterpret_cast<CD3D11RandomAccessResource*>( handle.m_resource.Get( ) );
}

CD3D11Texture1D* CD3D11ResourceManager::GetTexture1D( RE_HANDLE handle ) const
{
	assert( IsTexture1DHandle( handle ) );
	return reinterpret_cast<CD3D11Texture1D*>( handle.m_resource.Get( ) );
}

CD3D11Texture2D* CD3D11ResourceManager::GetTexture2D( RE_HANDLE handle ) const
{
	assert( IsTexture2DHandle( handle ) );
	return reinterpret_cast<CD3D11Texture2D*>( handle.m_resource.Get( ) );
}

CD3D11Texture3D* CD3D11ResourceManager::GetTexture3D( RE_HANDLE handle ) const
{
	assert( IsTexture3DHandle( handle ) );
	return reinterpret_cast<CD3D11Texture3D*>( handle.m_resource.Get( ) );
}

CD3D11Buffer* CD3D11ResourceManager::GetBuffer( RE_HANDLE handle ) const
{
	assert( IsBufferHandle( handle ) );
	return reinterpret_cast<CD3D11Buffer*>( handle.m_resource.Get( ) );
}

CD3D11VertexLayout* CD3D11ResourceManager::GetVertexLayout( RE_HANDLE handle )
{
	assert( IsVertexLayout( handle ) );
	return reinterpret_cast<CD3D11VertexLayout*>( handle.m_resource.Get( ) );
}

CD3D11VertexShader* CD3D11ResourceManager::GetVertexShader( RE_HANDLE handle ) const
{
	assert( IsVertexShaderHandle( handle ) );
	return reinterpret_cast<CD3D11VertexShader*>( handle.m_resource.Get( ) );
}

CD3D11GeometryShader* CD3D11ResourceManager::GetGeometryShader( RE_HANDLE handle ) const
{
	assert( IsGeometryShaderHandle( handle ) );
	return reinterpret_cast<CD3D11GeometryShader*>( handle.m_resource.Get( ) );
}

CD3D11PixelShader* CD3D11ResourceManager::GetPixelShader( RE_HANDLE handle ) const
{
	assert( IsPixelShaderHandle( handle ) );
	return reinterpret_cast<CD3D11PixelShader*>( handle.m_resource.Get( ) );
}

CD3D11ComputeShader* CD3D11ResourceManager::GetComputeShader( RE_HANDLE handle ) const
{
	assert( IsComputeShaderHandle( handle ) );
	return reinterpret_cast<CD3D11ComputeShader*>( handle.m_resource.Get( ) );
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

ID3D11Resource* CD3D11ResourceManager::GetD3D11ResourceGeneric( RE_HANDLE handle )
{
	if ( IsTexture1DHandle( handle ) )
	{
		CD3D11Texture1D* texture = GetTexture1D( handle );
		return texture->Get( );
	}
	else if ( IsTexture2DHandle( handle ) )
	{
		CD3D11Texture2D* texture = GetTexture2D( handle );
		return texture->Get( );
	}
	else if ( IsTexture3DHandle( handle ) )
	{
		CD3D11Texture3D* texture = GetTexture3D( handle );
		return texture->Get( );
	}
	else if ( IsBufferHandle( handle ) )
	{
		CD3D11Buffer* buffer = GetBuffer( handle );
		return buffer->Get( );
	}

	return nullptr;
}

IDeviceDependant* CD3D11ResourceManager::GetGraphicsResource( RE_HANDLE handle ) const
{
	GraphicsResourceType resourceType = handle.m_type;

	switch ( resourceType )
	{
	case GraphicsResourceType::BUFFER:
		{
			return reinterpret_cast<CD3D11Buffer*>( handle.m_resource.Get( ) );
		}
		break;
	case GraphicsResourceType::TEXTURE1D:
		{
			return reinterpret_cast<CD3D11Texture1D*>( handle.m_resource.Get( ) );
		}
		break;
	case GraphicsResourceType::TEXTURE2D:
		{
			return reinterpret_cast<CD3D11Texture2D*>( handle.m_resource.Get( ) );
		}
	break;
	case GraphicsResourceType::TEXTURE3D:
		{
			return reinterpret_cast<CD3D11Texture3D*>( handle.m_resource.Get( ) );
		}
	break;
	case GraphicsResourceType::SHADER_RESOURCE:
		{
			return reinterpret_cast<CD3D11ShaderResource*>( handle.m_resource.Get( ) );
		}
		break;
	case GraphicsResourceType::RANDOM_ACCESS:
		{
			return reinterpret_cast<CD3D11RandomAccessResource*>( handle.m_resource.Get( ) );
		}
		break;
	case GraphicsResourceType::RENDER_TARGET:
		{
			return reinterpret_cast<CD3D11RenderTarget*>( handle.m_resource.Get( ) );
		}
		break;
	case GraphicsResourceType::DEPTH_STENCIL:
		{
			return reinterpret_cast<CD3D11DepthStencil*>( handle.m_resource.Get( ) );
		}
		break;
	case GraphicsResourceType::VERTEX_LAYOUT:
		{
			return reinterpret_cast<CD3D11VertexLayout*>( handle.m_resource.Get( ) );
		}
		break;
	case GraphicsResourceType::VERTEX_SHADER:
		{
			return reinterpret_cast<CD3D11VertexShader*>( handle.m_resource.Get( ) );
		}
		break;
	case GraphicsResourceType::GEOMETRY_SHADER:
		{
			return reinterpret_cast<CD3D11GeometryShader*>( handle.m_resource.Get( ) );
		}
		break;
	case GraphicsResourceType::PIXEL_SHADER:
		{
			return reinterpret_cast<CD3D11PixelShader*>( handle.m_resource.Get( ) );
		}
		break;
	case GraphicsResourceType::COMPUTE_SHADER:
		{
			return reinterpret_cast<CD3D11ComputeShader*>( handle.m_resource.Get( ) );
		}
		break;
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
