#include "D3D11ResourceViews.h"

#include "D3D11Api.h"

namespace agl
{
	void D3D11ShaderResourceView::InitResource( )
	{
		[[maybe_unused]] HRESULT hr = D3D11Device( ).CreateShaderResourceView( m_d3d11Resource, &m_d3dDesc, &m_resource );
		assert( SUCCEEDED( hr ) );
	}

	void D3D11UnorderedAccessView::InitResource( )
	{
		[[maybe_unused]] HRESULT hr = D3D11Device( ).CreateUnorderedAccessView( m_d3d11Resource, &m_d3dDesc, &m_resource );
		assert( SUCCEEDED( hr ) );
	}

	D3D11RenderTargetView::D3D11RenderTargetView( IResourceViews* viewHolder, ID3D11Resource* d3d11Resource, const D3D11_RENDER_TARGET_VIEW_DESC& desc, const ColorF& clearColor )
		: BaseClass( viewHolder, d3d11Resource, desc )
		, m_clearColor( clearColor )
	{
	}

	ColorF D3D11RenderTargetView::GetClearColor() const
	{
		return m_clearColor;
	}

	void D3D11RenderTargetView::InitResource( )
	{
		[[maybe_unused]] HRESULT hr = D3D11Device( ).CreateRenderTargetView( m_d3d11Resource, &m_d3dDesc, &m_resource );
		assert( SUCCEEDED( hr ) );
	}

	D3D11DepthStencilView::D3D11DepthStencilView( IResourceViews* viewHolder, ID3D11Resource* d3d11Resource, const D3D11_DEPTH_STENCIL_VIEW_DESC& desc, float depthClearValue, uint8 stencilClearValue )
		: BaseClass( viewHolder, d3d11Resource, desc )
		, m_depthClearValue( depthClearValue )
		, m_stencilClearValue( stencilClearValue )
	{
	}

	float D3D11DepthStencilView::GetDepthClearValue() const
	{
		return m_depthClearValue;
	}

	uint8 D3D11DepthStencilView::GetStencilClearValue() const
	{
		return m_stencilClearValue;
	}

	void D3D11DepthStencilView::InitResource( )
	{
		[[maybe_unused]] HRESULT hr = D3D11Device( ).CreateDepthStencilView( m_d3d11Resource, &m_d3dDesc, &m_resource );
		assert( SUCCEEDED( hr ) );
	}
}
