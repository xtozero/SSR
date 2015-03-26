#include "stdafx.h"
#include "common.h"
#include "Direct3D11.h"

bool CDirect3D11::InitializeRenderer ( HWND hWind, UINT nWndWidth, UINT nWndHeight )
{
	ON_FAIL_RETURN ( CreateD3D11Device ( hWind, nWndWidth, nWndHeight ) );
	ON_FAIL_RETURN ( CreatePrimeRenderTargetVIew () );
	ON_FAIL_RETURN ( CreatePrimeDepthBuffer ( nWndWidth, nWndHeight ) );
	ON_FAIL_RETURN ( SetRenderTargetAndDepthBuffer () );

	return true;
}

void CDirect3D11::ShutDownRenderer ()
{
	SAFE_RELEASE ( m_pd3d11DeviceContext );
	SAFE_RELEASE ( m_pd3d11Device );
	SAFE_RELEASE ( m_pdxgiSwapChain );
	SAFE_RELEASE ( m_pd3d11PrimeRTView );
	SAFE_RELEASE ( m_pd3d11PrimeDSBuffer );
	SAFE_RELEASE ( m_pd3d11PrimeDSView );
}

void CDirect3D11::ClearRenderTargetView ( )
{
	ClearRenderTargetView ( 1.0f, 1.0f, 1.0f, 1.0f );
}

void CDirect3D11::ClearRenderTargetView ( float r, float g, float b, float a )
{
	const float clearColor[4] = { r, g, b, a };

	m_pd3d11DeviceContext->ClearRenderTargetView ( m_pd3d11PrimeRTView, clearColor );
}

void CDirect3D11::ClearDepthStencilView ( )
{
	m_pd3d11DeviceContext->ClearDepthStencilView ( m_pd3d11PrimeDSView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0 );
}

void CDirect3D11::Present ( )
{
	m_pdxgiSwapChain->Present ( 0, 0 );
}

void CDirect3D11::Render ()
{
	ClearDepthStencilView ();
	ClearRenderTargetView ();
	// TO DO SOMETHING DRAW
	Present ();
}

bool CDirect3D11::CreateD3D11Device ( HWND hWind, UINT nWndWidth, UINT nWndHeight )
{
	D3D_DRIVER_TYPE d3dDriverTypes[] = {
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE
	};

	D3D_FEATURE_LEVEL d3dFeatureLevel[] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};

	UINT flag = 0;
#ifdef _DEBUG
	flag |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	DXGI_SWAP_CHAIN_DESC dxgiSwapchainDesc;
	::ZeroMemory ( &dxgiSwapchainDesc, sizeof(dxgiSwapchainDesc) );

	dxgiSwapchainDesc.BufferCount = 1;
	dxgiSwapchainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapchainDesc.BufferDesc.Height = nWndHeight;
	dxgiSwapchainDesc.BufferDesc.Width = nWndWidth;
	dxgiSwapchainDesc.BufferDesc.RefreshRate.Denominator = 1;
	dxgiSwapchainDesc.BufferDesc.RefreshRate.Numerator = 60;
	dxgiSwapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapchainDesc.OutputWindow = hWind;
	dxgiSwapchainDesc.SampleDesc.Count = 1;
	dxgiSwapchainDesc.SampleDesc.Quality = 0;
	dxgiSwapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	dxgiSwapchainDesc.Windowed = true;

	D3D_FEATURE_LEVEL selectedFeature = D3D_FEATURE_LEVEL_11_0;

	HRESULT hr;

	for ( int i = 0; i < _countof ( d3dDriverTypes ); ++i )
	{
		if ( SUCCEEDED ( hr = D3D11CreateDeviceAndSwapChain ( NULL,
			d3dDriverTypes[i],
			NULL,
			flag,
			d3dFeatureLevel,
			_countof ( d3dFeatureLevel ),
			D3D11_SDK_VERSION,
			&dxgiSwapchainDesc,
			&m_pdxgiSwapChain,
			&m_pd3d11Device,
			&selectedFeature,
			&m_pd3d11DeviceContext
			) ) )
		{
			return true;
		}
	}

	return false;
}

bool CDirect3D11::CreatePrimeRenderTargetVIew ()
{
	ID3D11Texture2D* pd3d11BackBuffer;

	if ( SUCCEEDED ( m_pdxgiSwapChain->GetBuffer ( 0, __uuidof(ID3D11Texture2D), (LPVOID*)&pd3d11BackBuffer ) ) )
	{
		HRESULT hr = m_pd3d11Device->CreateRenderTargetView ( pd3d11BackBuffer, NULL, &m_pd3d11PrimeRTView );

		SAFE_RELEASE ( pd3d11BackBuffer );

		if ( SUCCEEDED ( hr ) )
		{
			return true;
		}
	}

	return false;
}

bool CDirect3D11::CreatePrimeDepthBuffer ( UINT nWndWidth, UINT nWndHeight )
{
	D3D11_TEXTURE2D_DESC d3d11Texture2DDesc;
	::ZeroMemory ( &d3d11Texture2DDesc, sizeof(d3d11Texture2DDesc) );

	d3d11Texture2DDesc.ArraySize = 1;
	d3d11Texture2DDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	d3d11Texture2DDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3d11Texture2DDesc.Height = nWndWidth;
	d3d11Texture2DDesc.Width = nWndHeight;
	d3d11Texture2DDesc.MipLevels = 1;
	d3d11Texture2DDesc.SampleDesc.Count = 1;
	d3d11Texture2DDesc.SampleDesc.Quality = 0;
	d3d11Texture2DDesc.Usage = D3D11_USAGE_DEFAULT;

	if ( SUCCEEDED ( m_pd3d11Device->CreateTexture2D ( &d3d11Texture2DDesc, NULL, &m_pd3d11PrimeDSBuffer ) ) )
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC d3d11DSDesc;
		::ZeroMemory ( &d3d11DSDesc, sizeof(d3d11DSDesc) );

		d3d11DSDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		d3d11DSDesc.Texture2D.MipSlice = 0;
		d3d11DSDesc.Format = d3d11Texture2DDesc.Format;

		if ( SUCCEEDED ( m_pd3d11Device->CreateDepthStencilView ( m_pd3d11PrimeDSBuffer, &d3d11DSDesc, &m_pd3d11PrimeDSView ) ) )
		{
			return true;
		}
	}

	return false;
}

bool CDirect3D11::SetRenderTargetAndDepthBuffer ( )
{
	if ( m_pd3d11PrimeRTView && m_pd3d11PrimeDSView )
	{
		m_pd3d11DeviceContext->OMSetRenderTargets ( 1, &m_pd3d11PrimeRTView, m_pd3d11PrimeDSView );
		return true;
	}
	else
	{
		return false;
	}
}

CDirect3D11::CDirect3D11 ( ) : m_pd3d11Device ( NULL ),
m_pd3d11DeviceContext ( NULL ),
m_pdxgiSwapChain ( NULL ),
m_pd3d11PrimeRTView ( NULL ),
m_pd3d11PrimeDSBuffer ( NULL ),
m_pd3d11PrimeDSView ( NULL )
{
}


CDirect3D11::~CDirect3D11 ()
{
}