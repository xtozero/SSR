#include "stdafx.h"
#include "D3D11ResourceViews.h"

#include "D3D11Api.h"

namespace aga
{
	void D3D11ShaderResourceView::InitResource( )
	{
		HRESULT hr = D3D11Device( ).CreateShaderResourceView( m_resourceRef, &m_desc, &m_view );
		assert( SUCCEEDED( hr ) );
	}

	void D3D11UnorderedAccessView::InitResource( )
	{
		HRESULT hr = D3D11Device( ).CreateUnorderedAccessView( m_resourceRef, &m_desc, &m_view );
		assert( SUCCEEDED( hr ) );
	}

	void D3D11RenderTargetView::InitResource( )
	{
		HRESULT hr = D3D11Device( ).CreateRenderTargetView( m_resourceRef, &m_desc, &m_view );
		assert( SUCCEEDED( hr ) );
	}

	void D3D11DepthStencilView::InitResource( )
	{
		HRESULT hr = D3D11Device( ).CreateDepthStencilView( m_resourceRef, &m_desc, &m_view );
		assert( SUCCEEDED( hr ) );
	}
}
