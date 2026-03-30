#include "D3D11Viewport.h"

#include "D3D11Api.h"
#include "D3D11ResourceViews.h"
#include "D3D11Texture.h"
#include "DxgiFlagConvertor.h"
#include "DxgiSwapchain.h"
#include "IAgl.h"
#include "ICommandList.h"
#include "LibraryTool/InterfaceFactories.h"
#include "TaskScheduler.h"
#include "Texture.h"

#include <d3d11.h>

namespace agl
{
	void D3D11Viewport::Clear()
	{
		if ( m_frameBuffer.Get() != nullptr )
		{
			if ( RenderTargetView* rtv = m_frameBuffer->RTV() )
			{
				ICommandList* commandList = GetInterface<IAgl>()->GetCommandList();
				commandList->ClearRenderTarget( rtv );
			}
		}
	}

	void D3D11Viewport::Bind( ICommandList& commandList ) const
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

	std::pair<uint32, uint32> D3D11Viewport::SizeOnRenderThread() const
	{
		assert( IsInRenderThread() );
		return { m_proxy.m_width, m_proxy.m_height };
	}

	void D3D11Viewport::Resize( uint32 width, uint32 height )
	{
		if ( ( m_width == width ) && ( m_height == height ) )
		{
			return;
		}

		m_width = width;
		m_height = height;

		if ( m_swapchain.Get() == nullptr )
		{
			CreateDedicateTexture();
		}
	}

	D3D11Viewport::D3D11Viewport( uint32 width, uint32 height, DXGI_FORMAT format, const float4& clearColor )
		: m_width( width )
		, m_height( height )
		, m_format( format )
		, m_clearColor{ clearColor[0], clearColor[1], clearColor[2], clearColor[3] }
	{
		m_proxy.m_width = m_width;
		m_proxy.m_height = m_height;

		CreateDedicateTexture();
	}

	D3D11Viewport::D3D11Viewport( DxgiSwapchain<AglType::D3D11>& swapchain )
		: m_width( swapchain.Width() )
		, m_height( swapchain.Height() )
		, m_format( swapchain.Format() )
		, m_clearColor{}
		, m_swapchain( &swapchain )
	{
		m_proxy.m_width = m_width;
		m_proxy.m_height = m_height;
	}

	Texture* D3D11Viewport::Texture()
	{
		return ( m_swapchain.Get() != nullptr ) ? m_swapchain->Texture() : m_frameBuffer.Get();
	}

	void D3D11Viewport::InitResource()
	{
	}

	void D3D11Viewport::FreeResource()
	{
		m_frameBuffer = nullptr;
		m_swapchain = nullptr;
	}

	void D3D11Viewport::CreateDedicateTexture()
	{
		ResourceFormat orignalFormat = ConvertDxgiFormatToFormat( m_format );
		DXGI_FORMAT typelessDxgiFormat = ConvertDxgiFormatToDxgiTypelessFormat( m_format );
		ResourceFormat typelessFormat = ConvertDxgiFormatToFormat( typelessDxgiFormat );

		TextureDesc frameBufferDesc = {
			.m_width = m_width,
			.m_height = m_height,
			.m_depth = 1,
			.m_sampleCount = 1,
			.m_sampleQuality = 0,
			.m_mipLevels = 1,
			.m_format = typelessFormat,
			.m_access = ResourceAccess::Default,
			.m_bindType = ResourceBindType::RenderTarget | ResourceBindType::ShaderResource,
			.m_miscFlag = ResourceMisc::WithoutViews,
			.m_clearValue = ResourceClearValue{
				.m_format = orignalFormat,
				.m_color = { m_clearColor[0], m_clearColor[1], m_clearColor[2], m_clearColor[3]}
			}
		};

		if ( m_frameBuffer == nullptr )
		{
			m_frameBuffer = new D3D11Texture2D( frameBufferDesc, "FrameBuffer", ResourceState::Common, nullptr );
		}
		else
		{
			m_frameBuffer->Reconstruct( frameBufferDesc, nullptr );
		}

		EnqueueRenderTask(
			[this, orignalFormat]()
			{
				m_frameBuffer->Free();
				m_frameBuffer->Init();

				m_frameBuffer->CreateRenderTarget( orignalFormat );
				m_frameBuffer->CreateShaderResource( orignalFormat );

				const TextureDesc& desc = m_frameBuffer->GetDesc();
				m_proxy.m_width = desc.m_width;
				m_proxy.m_height = desc.m_height;
			} );
	}
}