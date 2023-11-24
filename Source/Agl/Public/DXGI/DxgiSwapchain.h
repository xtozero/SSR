#pragma once

#include "Canvas.h"
#include "D3D11Texture.h"
#include "D3D12Texture.h"
#include "DXGIFlagConvertor.h"
#include "IAgl.h"
#include "ICommandList.h"
#include "Memory/InlineMemoryAllocator.h"
#include "Texture.h"

#include <dxgi1_6.h>
#include <wrl/client.h>

namespace agl
{
	template <AglType Backend>
	class DxgiSwapchain final : public Canvas
	{
	public:
		virtual void OnBeginFrameRendering() override
		{
			GetInterface<IAgl>()->OnBeginFrameRendering();

			agl::Texture* backBuffer = Texture();
			if ( backBuffer == nullptr )
			{
				return;
			}

			agl::ResourceTransition transition
			{
				.m_pResource = backBuffer->Resource(),
				.m_pTransitionable = backBuffer,
				.m_subResource = agl::AllSubResource,
				.m_state = ResourceState::RenderTarget
			};

			ICommandList* commandList = GetInterface<IAgl>()->GetCommandList();
			commandList->Transition( 1, &transition );
		}

		virtual void OnEndFrameRendering() override
		{
			agl::Texture* backBuffer = Texture();
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

		virtual DeviceError Present( bool vSync = false ) override
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

		virtual void Clear( const float( &clearColor )[4] ) override
		{
			ICommandList* commandList = GetInterface<IAgl>()->GetCommandList();

			if ( auto curTex = Texture() )
			{
				if ( auto rtv = curTex->RTV() )
				{
					commandList->ClearRenderTarget( rtv, clearColor );
				}
			}
		}

		virtual void* Handle() const override
		{
			return m_hWnd;
		}

		virtual std::pair<uint32, uint32> Size() const
		{
			return { m_width, m_height };
		}

		virtual void Resize( const std::pair<uint32, uint32>& newSize ) override
		{
			GetInterface<agl::IAgl>()->WaitGPU();

			m_width = newSize.first;
			m_height = newSize.second;

			for ( auto& backBuffer : m_backBuffers )
			{
				backBuffer->Free();
			}

			DXGI_FORMAT format = m_format;
			if constexpr ( Backend != AglType::D3D11 )
			{
				format = ConvertToDxgiLinearFormat( m_format );
			}

			HRESULT hr = m_pSwapChain->ResizeBuffers( m_bufferCount, m_width, m_height, format, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH );
			assert( SUCCEEDED( hr ) );

			for ( uint32 i = 0; i < m_bufferCount; ++i )
			{
				int32 oldRefCount = m_backBuffers[i]->GetRefCount();

				if constexpr ( Backend == AglType::D3D11 )
				{
					ID3D11Texture2D* backBuffer = nullptr;
					hr = m_pSwapChain->GetBuffer( 0, IID_PPV_ARGS( &backBuffer ) );

					assert( SUCCEEDED( hr ) );

					std::construct_at( static_cast<D3D11Texture2D*>( m_backBuffers[i].Get() ), backBuffer, "SwapChain" );
				}
				else
				{
					ID3D12Resource* backBuffer = nullptr;
					hr = m_pSwapChain->GetBuffer( i, IID_PPV_ARGS( &backBuffer ) );

					assert( SUCCEEDED( hr ) );

					D3D12_RESOURCE_DESC desc = backBuffer->GetDesc();
					desc.Format = m_format;

					std::construct_at( static_cast<D3D12Texture2D*>( m_backBuffers[i].Get() ), backBuffer, "SwapChain", &desc );
				}

				m_backBuffers[i]->Init();

				while ( oldRefCount != m_backBuffers[i]->GetRefCount() )
				{
					m_backBuffers[i]->AddRef();
				}
			}
		}

		uint32 Width() const
		{
			return m_width;
		}

		uint32 Height() const
		{
			return m_height;
		}

		DXGI_FORMAT Format() const
		{
			return m_format;
		}

		virtual agl::Texture* Texture() override
		{
			return m_backBuffers[m_bufferIndex].Get();
		}

		DxgiSwapchain( IUnknown& device, IDXGIFactory7& factory, uint32 width, uint32 height, uint32 bufferCount, void* hWnd, DXGI_FORMAT format )
			: m_device( &device )
			, m_factory( &factory )
			, m_width( width )
			, m_height( height )
			, m_bufferCount( bufferCount )
			, m_hWnd( hWnd )
			, m_format( format )
		{}

	private:
		virtual void InitResource() override
		{
			DXGI_FORMAT format = m_format;
			if constexpr ( Backend != AglType::D3D11 )
			{
				format = ConvertToDxgiLinearFormat( m_format );
			}

			DXGI_SWAP_CHAIN_DESC dxgiSwapchainDesc = {
			.BufferDesc = {
				.Width = m_width,
				.Height = m_height,
				.RefreshRate = {
					.Numerator = 60,
					.Denominator = 1,
				},
				.Format = format,
				.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
				.Scaling = DXGI_MODE_SCALING_UNSPECIFIED,
			},
			.SampleDesc = {
				.Count = 1,
				.Quality = 0
			},
			.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT,
			.BufferCount = m_bufferCount,
			.OutputWindow = static_cast<HWND>( m_hWnd ),
			.Windowed = true,
			.SwapEffect = ( m_bufferCount > 1 ) ? DXGI_SWAP_EFFECT_FLIP_DISCARD : DXGI_SWAP_EFFECT_DISCARD,
			.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
			};

			Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
			HRESULT hr = m_factory->CreateSwapChain( m_device, &dxgiSwapchainDesc, swapChain.GetAddressOf() );

			assert( SUCCEEDED( hr ) );

			hr = swapChain.As( &m_pSwapChain );

			assert( SUCCEEDED( hr ) );

			m_bufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();
			m_backBuffers.resize( m_bufferCount );

			for ( uint32 i = 0; i < m_bufferCount; ++i )
			{
				if constexpr ( Backend == AglType::D3D11 )
				{
					ID3D11Texture2D* backBuffer = nullptr;
					hr = m_pSwapChain->GetBuffer( i, IID_PPV_ARGS( &backBuffer ) );

					assert( SUCCEEDED( hr ) );
					m_backBuffers[i] = new D3D11Texture2D( backBuffer, "SwapChain" );
				}
				else
				{
					ID3D12Resource* backBuffer = nullptr;
					hr = m_pSwapChain->GetBuffer( i, IID_PPV_ARGS( &backBuffer ) );

					assert( SUCCEEDED( hr ) );

					D3D12_RESOURCE_DESC desc = backBuffer->GetDesc();
					desc.Format = m_format;

					m_backBuffers[i] = new D3D12Texture2D( backBuffer, "SwapChain", &desc);
				}

				m_backBuffers[i]->Init();
			}
		}

		virtual void FreeResource() override
		{
			m_pSwapChain.Reset();
			m_backBuffers.clear();
		}

		IUnknown* m_device = nullptr;
		IDXGIFactory7* m_factory = nullptr;

		uint32 m_width = 0;
		uint32 m_height = 0;
		uint32 m_bufferCount = 0;
		void* m_hWnd = nullptr;
		DXGI_FORMAT m_format = DXGI_FORMAT_UNKNOWN;
		Microsoft::WRL::ComPtr<IDXGISwapChain4> m_pSwapChain;

		uint32 m_bufferIndex = 0;
		std::vector<RefHandle<agl::Texture>, InlineAllocator<RefHandle<agl::Texture>, 2>> m_backBuffers;
	};
}
