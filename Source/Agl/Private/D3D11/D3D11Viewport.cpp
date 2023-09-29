#include "stdafx.h"
#include "D3D11Viewport.h"

#include "D3D11Api.h"
#include "D3D11BaseTexture.h"
#include "D3D11ResourceViews.h"
#include "DxgiFlagConvertor.h"
#include "IAgl.h"
#include "ICommandList.h"
#include "LibraryTool/InterfaceFactories.h"
#include "TaskScheduler.h"
#include "Texture.h"

#include <d3d11.h>

namespace agl
{
	DeviceError D3D11Viewport::Present( bool vSync )
	{
		HRESULT hr = m_pSwapChain->Present( vSync ? 1 : 0, 0 );

		if ( hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET )
		{
			return DeviceError::DeviceLost;
		}

		return DeviceError::None;
	}

	void D3D11Viewport::Clear( const float (&clearColor)[4] )
	{
		ICommandList* commandList = GetInterface<IAgl>()->GetCommandList();

		if ( m_backBuffer.Get() != nullptr )
		{
			if ( agl::RenderTargetView* rtv = m_backBuffer->RTV() )
			{
				commandList->ClearRenderTarget( rtv, clearColor );
			}
		}

		if ( m_useDedicateTexture == false )
		{
			return;
		}

		if ( m_frameBuffer.Get() != nullptr )
		{
			if ( agl::RenderTargetView* rtv = m_frameBuffer->RTV() )
			{
				commandList->ClearRenderTarget( rtv, clearColor );
			}
		}
	}

	void D3D11Viewport::Bind( ICommandListBase& commandList ) const
	{
		CubeArea<float> viewport{ 
			.m_left = 0.f,
			.m_top = 0.f,
			.m_front = 0.f,
			.m_right = static_cast<float>( m_width ),
			.m_bottom = static_cast<float>( m_height ),
			.m_back = 1.f
		};
		commandList.SetViewports( 1, &viewport );

		RectangleArea<int32> rect{ 
			.m_left = 0L, 
			.m_top = 0L, 
			.m_right = static_cast<int32>( m_width ), 
			.m_bottom = static_cast<int32>( m_height ) 
		};
		commandList.SetScissorRects( 1, &rect );
	}

	std::pair<uint32, uint32> D3D11Viewport::Size() const
	{
		return { m_width, m_height };
	}

	void D3D11Viewport::Resize( const std::pair<uint32, uint32>& newSize )
	{
		m_width = newSize.first;
		m_height = newSize.second;

		m_backBuffer->Free();

		HRESULT hr = m_pSwapChain->ResizeBuffers( 1, m_width, m_height, m_format, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH );
		assert( SUCCEEDED( hr ) );

		ID3D11Texture2D* backBuffer = nullptr;
		hr = m_pSwapChain->GetBuffer( 0, IID_PPV_ARGS( &backBuffer ) );

		assert( SUCCEEDED( hr ) );
		int32 oldRefCount = m_backBuffer->GetRefCount();
		std::construct_at( m_backBuffer.Get(), backBuffer );
		m_backBuffer->Init();

		while ( oldRefCount != m_backBuffer->GetRefCount() )
		{
			m_backBuffer->AddRef();
		}

		if ( m_useDedicateTexture )
		{
			CreateDedicateTexture();
		}
	}

	D3D11Viewport::D3D11Viewport( uint32 width, uint32 height, void* hWnd, DXGI_FORMAT format, bool useDedicateTexture ) 
		: m_width( width )
		, m_height( height )
		, m_hWnd( hWnd )
		, m_format( format )
		, m_useDedicateTexture( useDedicateTexture )
	{
	}

	Texture* D3D11Viewport::Texture()
	{
		return m_useDedicateTexture
			? m_frameBuffer.Get()
			: m_backBuffer.Get();
	}

	agl::Texture* D3D11Viewport::Canvas()
	{
		return m_backBuffer.Get();
	}

	void D3D11Viewport::InitResource()
	{
		DXGI_SWAP_CHAIN_DESC dxgiSwapchainDesc = {};

		dxgiSwapchainDesc.BufferCount = 1;
		dxgiSwapchainDesc.BufferDesc.Format = m_format;
		dxgiSwapchainDesc.BufferDesc.Width = m_width;
		dxgiSwapchainDesc.BufferDesc.Height = m_height;
		dxgiSwapchainDesc.BufferDesc.RefreshRate.Denominator = 1;
		dxgiSwapchainDesc.BufferDesc.RefreshRate.Numerator = 60;
		dxgiSwapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
		dxgiSwapchainDesc.OutputWindow = static_cast<HWND>( m_hWnd );
		dxgiSwapchainDesc.SampleDesc.Count = 1;
		dxgiSwapchainDesc.SampleDesc.Quality = 0;
		dxgiSwapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		dxgiSwapchainDesc.Windowed = true;
		dxgiSwapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		auto& device = D3D11Device();
		auto& factory = D3D11Factory();
		HRESULT hr = factory.CreateSwapChain( &device, &dxgiSwapchainDesc, &m_pSwapChain );

		assert( SUCCEEDED( hr ) );

		ID3D11Texture2D* backBuffer = nullptr;
		hr = m_pSwapChain->GetBuffer( 0, IID_PPV_ARGS( &backBuffer ) );

		assert( SUCCEEDED( hr ) );
		m_backBuffer = new D3D11BaseTexture2D( backBuffer );
		m_backBuffer->Init();

		if ( m_useDedicateTexture )
		{
			CreateDedicateTexture();
		}
	}

	void D3D11Viewport::FreeResource()
	{
		m_backBuffer = nullptr;
		m_frameBuffer = nullptr;
		m_pSwapChain.Reset();
	}

	void D3D11Viewport::CreateDedicateTexture()
	{
		ResourceFormat orignalFormat = ConvertDxgiFormatToFormat( m_format );
		DXGI_FORMAT typelessDxgiFormat = ConvertDxgiFormatToDxgiTypelessFormat( m_format );
		ResourceFormat typelessFormat = ConvertDxgiFormatToFormat( typelessDxgiFormat );

		TextureTrait frameBufferTrait = {
			.m_width = m_width,
			.m_height = m_height,
			.m_depth = 1,
			.m_sampleCount = 1,
			.m_sampleQuality = 0,
			.m_mipLevels = 1,
			.m_format = typelessFormat,
			.m_access = ResourceAccessFlag::Default,
			.m_bindType = ResourceBindType::RenderTarget | ResourceBindType::ShaderResource,
			.m_miscFlag = ResourceMisc::WithoutViews
		};

		if ( m_frameBuffer == nullptr )
		{
			m_frameBuffer = new D3D11BaseTexture2D( frameBufferTrait, nullptr );
		}
		else
		{
			m_frameBuffer->Recreate( frameBufferTrait, nullptr );
		}

		EnqueueRenderTask( [this, orignalFormat]()
			{
				m_frameBuffer->Init();

				m_frameBuffer->CreateRenderTarget( orignalFormat );
				m_frameBuffer->CreateShaderResource( orignalFormat );
			} );
	}
}