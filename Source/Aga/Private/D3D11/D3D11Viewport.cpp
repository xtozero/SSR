#include "stdafx.h"
#include "D3D11Viewport.h"

#include "D3D11Api.h"
#include "D3D11BaseTexture.h"

#include "Texture.h"

#include <d3d11.h>

namespace aga
{
	DEVICE_ERROR D3D11Viewport::Present( bool vSync )
	{
		HRESULT hr = m_pSwapChain->Present( vSync ? 1 : 0, 0 );

		if ( hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET )
		{
			return DEVICE_ERROR::DEVICE_LOST;
		}

		return DEVICE_ERROR::NONE;
	}

	void D3D11Viewport::Clear( const float( &clearColor )[4] )
	{
		if ( m_backBuffer.Get( ) )
		{
			ID3D11RenderTargetView* rtv = m_backBuffer->RenderTargetView( );
			if ( rtv )
			{
				D3D11Context( ).ClearRenderTargetView( rtv, clearColor );
			}
		}
	}

	std::pair<UINT, UINT> D3D11Viewport::Size( ) const
	{
		return { m_width, m_height };
	}

	void D3D11Viewport::Resize( const std::pair<UINT, UINT>& newSize )
	{
		m_width = newSize.first;
		m_height = newSize.second;

		m_backBuffer->Free( );

		HRESULT hr = m_pSwapChain->ResizeBuffers( 1, m_width, m_height, m_format, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH );
		assert( SUCCEEDED( hr ) );

		ID3D11Texture2D* backBuffer = nullptr;
		hr = m_pSwapChain->GetBuffer( 0, IID_PPV_ARGS( &backBuffer ) );

		assert( SUCCEEDED( hr ) );
		new ( m_backBuffer.Get( ) )D3D11BaseTexture2D( backBuffer );
		m_backBuffer->Init( );
	}

	D3D11Viewport::D3D11Viewport( int width, int height, void* hWnd, DXGI_FORMAT format ) :
		m_width( static_cast<UINT>( width ) ), m_height( static_cast<UINT>( height ) ), m_hWnd( hWnd ), m_format( format )
	{
	}

	aga::Texture* D3D11Viewport::Texture( )
	{
		return m_backBuffer.Get( );
	}

	void D3D11Viewport::InitResource( )
	{
		DXGI_SWAP_CHAIN_DESC dxgiSwapchainDesc = {};

		dxgiSwapchainDesc.BufferCount = 1;
		dxgiSwapchainDesc.BufferDesc.Format = m_format;
		dxgiSwapchainDesc.BufferDesc.Width = m_width;
		dxgiSwapchainDesc.BufferDesc.Height = m_height;
		dxgiSwapchainDesc.BufferDesc.RefreshRate.Denominator = 1;
		dxgiSwapchainDesc.BufferDesc.RefreshRate.Numerator = 60;
		dxgiSwapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
		dxgiSwapchainDesc.OutputWindow = static_cast<HWND>(m_hWnd);
		dxgiSwapchainDesc.SampleDesc.Count = 1;
		dxgiSwapchainDesc.SampleDesc.Quality = 0;
		dxgiSwapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		dxgiSwapchainDesc.Windowed = true;
		dxgiSwapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		auto& device = D3D11Device( );
		auto& factory = D3D11Factory( );
		HRESULT hr = factory.CreateSwapChain( &device, &dxgiSwapchainDesc, &m_pSwapChain );

		assert( SUCCEEDED( hr ) );

		ID3D11Texture2D* backBuffer = nullptr;
		hr = m_pSwapChain->GetBuffer( 0, IID_PPV_ARGS( &backBuffer ) );

		assert( SUCCEEDED( hr ) );
		m_backBuffer = new D3D11BaseTexture2D( backBuffer );
		m_backBuffer->Init( );
	}

	void D3D11Viewport::FreeResource( )
	{
		m_backBuffer = nullptr;
		m_pSwapChain.Reset( );
	}
}