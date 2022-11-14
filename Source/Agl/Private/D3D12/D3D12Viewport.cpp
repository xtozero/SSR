#include "D3D12Viewport.h"

#include "CommandLists.h"
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
		D3D12BaseTexture2D* backBuffer = m_backBuffers[m_bufferIndex].Get();
		if ( backBuffer == nullptr )
		{
			return;
		}

		GraphicsCommandListsBase& comandLists = GetInterface<IAgl>()->GetGraphicsCommandLists();
		IGraphicsCommandList* commandList = comandLists.GetCommandList( 0 );

		backBuffer->Transition( *commandList, ResourceState::RenderTarget );
	}

	void D3D12Viewport::OnEndFrameRendering()
	{
		D3D12BaseTexture2D* backBuffer = m_backBuffers[m_bufferIndex].Get();
		if ( backBuffer == nullptr )
		{
			return;
		}

		GraphicsCommandListsBase& comandLists = GetInterface<IAgl>()->GetGraphicsCommandLists();
		IGraphicsCommandList* commandList = comandLists.GetCommandList( 0 );

		backBuffer->Transition( *commandList, ResourceState::Present );
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
		if ( m_backBuffers[m_bufferIndex].Get() == nullptr )
		{
			return;
		}

		auto d3d12RTV = static_cast<D3D12RenderTargetView*>( m_backBuffers[m_bufferIndex]->RTV() );
		if ( d3d12RTV == nullptr )
		{
			return;
		}

		GraphicsCommandListsBase& comandLists = GetInterface<IAgl>()->GetGraphicsCommandLists();
		IGraphicsCommandList* commandList = comandLists.GetCommandList( 0 );

		commandList->ClearRenderTarget( d3d12RTV, clearColor);
	}

	void D3D12Viewport::Bind( ICommandList& commandList ) const
	{
		CubeArea<float> viewport{ 0.f, 0.f, static_cast<float>( m_width ), static_cast<float>( m_height ), 0.f, 1.f };
		commandList.SetViewports( 1, &viewport );

		RectangleArea<int32> rect{ 0L, 0L, static_cast<int32>( m_width ), static_cast<int32>( m_height ) };
		commandList.SetScissorRects( 1, &rect );
	}

	std::pair<uint32, uint32> D3D12Viewport::Size() const
	{
		return { m_width, m_height };
	}

	void D3D12Viewport::Resize( const std::pair<uint32, uint32>& newSize )
	{
	}

	agl::Texture* D3D12Viewport::Texture()
	{
		return m_backBuffers[m_bufferIndex].Get();
	}

	D3D12Viewport::D3D12Viewport( uint32 width, uint32 height, uint32 bufferCount, void* hWnd, DXGI_FORMAT format ) : m_width( width )
		, m_height( height )
		, m_bufferCount( bufferCount )
		, m_hWnd( hWnd )
		, m_format( format )
	{
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

		GraphicsCommandListsBase& comandLists = GetInterface<IAgl>()->GetGraphicsCommandLists();
		IGraphicsCommandList* commandList = comandLists.GetCommandList( 0 );

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
}
