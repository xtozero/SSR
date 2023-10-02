#include "D3D12Viewport.h"

#include "D3D12Api.h"
#include "D3D12ResourceViews.h"
#include "DxgiFlagConvertor.h"
#include "DxgiSwapchain.h"
#include "IAgl.h"
#include "ICommandList.h"
#include "LibraryTool/InterfaceFactories.h"

#include <d3d12.h>

using ::Microsoft::WRL::ComPtr;

namespace agl
{
	void D3D12Viewport::Clear( const float( &clearColor )[4] )
	{
		agl::Texture* backBuffer = m_frameBuffer.Get();
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

		if ( m_frameBuffer.Get() != nullptr )
		{
			if ( agl::RenderTargetView* rtv = m_frameBuffer->RTV() )
			{
				commandList->ClearRenderTarget( rtv, clearColor );
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
		if ( ( m_width == newSize.first ) && ( m_height == newSize.second ) )
		{
			return;
		}

		GetInterface<agl::IAgl>()->WaitGPU();

		m_width = newSize.first;
		m_height = newSize.second;

		if ( m_swapchain.Get() == nullptr )
		{
			CreateDedicateTexture();
		}
	}

	agl::Texture* D3D12Viewport::Texture()
	{
		return ( m_swapchain.Get() != nullptr )
			? m_swapchain->Texture()
			: m_frameBuffer.Get();
	}

	D3D12Viewport::D3D12Viewport( uint32 width, uint32 height, DXGI_FORMAT format, const float4& bgColor )
		: m_width( width )
		, m_height( height )
		, m_format( format )
		, m_clearColor{ bgColor[0], bgColor[1], bgColor[2], bgColor[3] }
	{
		CreateDedicateTexture();
	}

	D3D12Viewport::D3D12Viewport( DxgiSwapchain<AglType::D3D12>& swapchain )
		: m_width( swapchain.Width() )
		, m_height( swapchain.Height() )
		, m_format( swapchain.Format() )
		, m_clearColor{}
		, m_swapchain( &swapchain )
	{
	}

	void D3D12Viewport::InitResource()
	{
	}

	void D3D12Viewport::FreeResource()
	{
		m_frameBuffer = nullptr;
		m_swapchain = nullptr;
	}

	void D3D12Viewport::CreateDedicateTexture()
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
			.m_miscFlag = ResourceMisc::WithoutViews,
			.m_clearValue = agl::ResourceClearValue{
				.m_format = orignalFormat,
				.m_color = { m_clearColor[0], m_clearColor[1], m_clearColor[2], m_clearColor[3] }
			}
		};

		if ( m_frameBuffer == nullptr )
		{
			m_frameBuffer = new D3D12BaseTexture2D( frameBufferTrait, nullptr );
		}
		else
		{
			m_frameBuffer->Reconstruct( frameBufferTrait, nullptr );
		}

		EnqueueRenderTask( [this, orignalFormat]()
			{
				m_frameBuffer->Free();
				m_frameBuffer->Init();

				m_frameBuffer->CreateRenderTarget( orignalFormat );
				m_frameBuffer->CreateShaderResource( orignalFormat );
			} );
	}
}
