#include "D3D12Viewport.h"

#include "D3D12Api.h"
#include "D3D12ResourceViews.h"
#include "DXGIFlagConvertor.h"
#include "IAgl.h"
#include "ICommandList.h"
#include "LibraryTool/InterfaceFactories.h"

#include <d3d12.h>

using ::Microsoft::WRL::ComPtr;

namespace agl
{
	void D3D12Viewport::OnBeginFrameRendering()
	{
		GetInterface<IAgl>()->OnBeginFrameRendering();

		std::vector<agl::ResourceTransition, InlineAllocator<agl::ResourceTransition, 2>> transitions;

		agl::ResourceTransition transition
		{
			.m_pResource = nullptr,
			.m_pTransitionable = nullptr,
			.m_subResource = agl::AllSubResource,
			.m_state = ResourceState::RenderTarget
		};

		if ( D3D12BaseTexture2D* backBuffer = m_backBuffers[m_bufferIndex].Get() )
		{
			transition.m_pResource = backBuffer->Resource();
			transition.m_pTransitionable = backBuffer;

			transitions.push_back( transition );
		}

		if ( m_useDedicateTexture )
		{
			if ( D3D12BaseTexture2D* backBuffer = m_frameBuffers[m_bufferIndex].Get() )
			{
				transition.m_pResource = backBuffer->Resource();
				transition.m_pTransitionable = backBuffer;

				transitions.push_back( transition );
			}
		}

		ICommandList* commandList = GetInterface<IAgl>()->GetCommandList();
		commandList->Transition( static_cast<uint32>( transitions.size() ), transitions.data());
	}

	void D3D12Viewport::OnEndFrameRendering()
	{
		D3D12BaseTexture2D* backBuffer = m_backBuffers[m_bufferIndex].Get();
		if ( backBuffer == nullptr )
		{
			return;
		}

		agl::ResourceTransition transition
		{
			.m_pResource = backBuffer->Resource(),
			.m_pTransitionable = backBuffer,
			.m_subResource = agl::AllSubResource,
			.m_state = ResourceState::Present
		};

		ICommandList* commandList = GetInterface<IAgl>()->GetCommandList();
		commandList->Transition( 1, &transition );
	}

	DeviceError D3D12Viewport::Present( bool vSync )
	{
		HRESULT hr = m_pSwapChain->Present( vSync ? 1 : 0, 0 );

		if ( hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET )
		{
			return DeviceError::DeviceLost;
		}

		uint32 oldBufferIndex = m_bufferIndex;
		m_bufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();
		GetInterface<IAgl>()->OnEndFrameRendering( oldBufferIndex, m_bufferIndex );

		return DeviceError::None;
	}

	void D3D12Viewport::Clear( const float( &clearColor )[4] )
	{
		ICommandList* commandList = GetInterface<IAgl>()->GetCommandList();

		if ( m_backBuffers[m_bufferIndex].Get() != nullptr )
		{
			if ( auto d3d12RTV = static_cast<D3D12RenderTargetView*>( m_backBuffers[m_bufferIndex]->RTV() ) )
			{
				commandList->ClearRenderTarget( d3d12RTV, clearColor );
			}
		}

		if ( m_useDedicateTexture == false )
		{
			return;
		}

		if ( m_frameBuffers[m_bufferIndex].Get() != nullptr )
		{
			if ( auto d3d12RTV = static_cast<D3D12RenderTargetView*>( m_frameBuffers[m_bufferIndex]->RTV() ) )
			{
				commandList->ClearRenderTarget( d3d12RTV, clearColor );
			}
		}
	}

	void D3D12Viewport::Bind( ICommandListBase& commandList ) const
	{
		CubeArea<float> viewport{ 
			.m_left= 0.f, 
			.m_top= 0.f, 
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

	std::pair<uint32, uint32> D3D12Viewport::Size() const
	{
		return { m_width, m_height };
	}

	void D3D12Viewport::Resize( const std::pair<uint32, uint32>& newSize )
	{
		GetInterface<agl::IAgl>()->WaitGPU();

		m_width = newSize.first;
		m_height = newSize.second;

		for ( auto& backBuffer : m_backBuffers )
		{
			backBuffer->Free();
		}

		HRESULT hr = m_pSwapChain->ResizeBuffers( m_bufferCount, m_width, m_height, ConvertToDxgiLinearFormat( m_format ), DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH );
		assert( SUCCEEDED( hr ) );

		for ( uint32 i = 0; i < m_bufferCount; ++i )
		{
			ID3D12Resource* backBuffer = nullptr;
			hr = m_pSwapChain->GetBuffer( i, IID_PPV_ARGS( &backBuffer ) );

			assert( SUCCEEDED( hr ) );

			int32 oldRefCount = m_backBuffers[i]->GetRefCount();

			D3D12_RESOURCE_DESC desc = backBuffer->GetDesc();
			desc.Format = m_format;

			std::construct_at( m_backBuffers[i].Get(), backBuffer, &desc );
			m_backBuffers[i]->Init();

			while ( oldRefCount != m_backBuffers[i]->GetRefCount() )
			{
				m_backBuffers[i]->AddRef();
			}
		}

		if ( m_useDedicateTexture )
		{
			CreateDedicateTexture();
		}
	}

	agl::Texture* D3D12Viewport::Texture()
	{
		return m_useDedicateTexture 
			? m_frameBuffers[m_bufferIndex].Get() 
			: m_backBuffers[m_bufferIndex].Get();
	}

	agl::Texture* D3D12Viewport::Canvas()
	{
		return m_backBuffers[m_bufferIndex].Get();
	}

	D3D12Viewport::D3D12Viewport( uint32 width, uint32 height, uint32 bufferCount, void* hWnd, DXGI_FORMAT format, const float4& bgColor, bool useDedicateTexture )
		: m_width( width )
		, m_height( height )
		, m_bufferCount( bufferCount )
		, m_hWnd( hWnd )
		, m_format( format )
		, m_clearColor{ bgColor[0], bgColor[1], bgColor[2], bgColor[3] }
		, m_useDedicateTexture( useDedicateTexture )
	{
		if ( m_useDedicateTexture )
		{
			CreateDedicateTexture();
		}
	}

	void D3D12Viewport::InitResource()
	{
		DXGI_SWAP_CHAIN_DESC dxgiSwapchainDesc = {};

		dxgiSwapchainDesc.BufferCount = m_bufferCount;
		dxgiSwapchainDesc.BufferDesc.Format = ConvertToDxgiLinearFormat( m_format );
		dxgiSwapchainDesc.BufferDesc.Width = m_width;
		dxgiSwapchainDesc.BufferDesc.Height = m_height;
		dxgiSwapchainDesc.BufferDesc.RefreshRate.Denominator = 1;
		dxgiSwapchainDesc.BufferDesc.RefreshRate.Numerator = 60;
		dxgiSwapchainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		dxgiSwapchainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		dxgiSwapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
		dxgiSwapchainDesc.OutputWindow = static_cast<HWND>( m_hWnd );
		dxgiSwapchainDesc.SampleDesc.Count = 1;
		dxgiSwapchainDesc.SampleDesc.Quality = 0;
		dxgiSwapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		dxgiSwapchainDesc.Windowed = true;
		dxgiSwapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		auto& directCommandQueue = D3D12DirectCommandQueue();
		auto& factory = D3D12Factory();
		ComPtr<IDXGISwapChain> swapChain;
		HRESULT hr = factory.CreateSwapChain( &directCommandQueue, &dxgiSwapchainDesc, swapChain.GetAddressOf() );
		
		assert( SUCCEEDED( hr ) );

		hr = swapChain.As( &m_pSwapChain );
		
		assert( SUCCEEDED( hr ) );

		m_bufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();
		m_backBuffers.resize( m_bufferCount );

		for ( uint32 i = 0; i < m_bufferCount; ++i )
		{
			ID3D12Resource* backBuffer = nullptr;
			hr = m_pSwapChain->GetBuffer( i, IID_PPV_ARGS( &backBuffer ) );
			
			assert( SUCCEEDED( hr ) );

			D3D12_RESOURCE_DESC desc = backBuffer->GetDesc();
			desc.Format = m_format;

			m_backBuffers[i] = new D3D12BaseTexture2D( backBuffer, &desc );
			m_backBuffers[i]->Init();
		}
	}

	void D3D12Viewport::FreeResource()
	{
		m_pSwapChain.Reset();
	}

	void D3D12Viewport::CreateDedicateTexture()
	{
		m_frameBuffers.resize( m_bufferCount );

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
			.m_miscFlag = ResourceMisc::WithoutViews,
			.m_clearValue = agl::ResourceClearValue{
				.m_format = orignalFormat,
				.m_color = { m_clearColor[0], m_clearColor[1], m_clearColor[2], m_clearColor[3] }
			}
		};

		for ( uint32 i = 0; i < m_bufferCount; ++i )
		{
			if ( m_frameBuffers[i] == nullptr )
			{
				m_frameBuffers[i] = new D3D12BaseTexture2D( frameBufferTrait, nullptr );
			}
			else
			{
				m_frameBuffers[i]->Recreate( frameBufferTrait, nullptr );
			}
		}

		EnqueueRenderTask( [this, orignalFormat]()
			{
				for ( uint32 i = 0; i < m_bufferCount; ++i )
				{
					m_frameBuffers[i]->Init();

					m_frameBuffers[i]->CreateRenderTarget( orignalFormat );
					m_frameBuffers[i]->CreateShaderResource( orignalFormat );
				}
			} );
	}
}
