#include "stdafx.h"
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
	void D3D11Viewport::Clear( const float (&clearColor)[4] )
	{
		if ( m_frameBuffer.Get() != nullptr )
		{
			if ( RenderTargetView* rtv = m_frameBuffer->RTV() )
			{
				ICommandList* commandList = GetInterface<IAgl>()->GetCommandList();
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

	std::pair<uint32, uint32> D3D11Viewport::SizeOnRenderThread() const
	{
		assert( IsInRenderThread() );
		return { m_proxy.m_width, m_proxy.m_height };
	}

	void D3D11Viewport::Resize( const std::pair<uint32, uint32>& newSize )
	{
		if ( ( m_width == newSize.first ) && ( m_height == newSize.second ) )
		{
			return;
		}

		m_width = newSize.first;
		m_height = newSize.second;

		if ( m_swapchain.Get() == nullptr )
		{
			CreateDedicateTexture();
		}
	}

	D3D11Viewport::D3D11Viewport( uint32 width, uint32 height, DXGI_FORMAT format ) 
		: m_width( width )
		, m_height( height )
		, m_format( format )
	{
		m_proxy.m_width = m_width;
		m_proxy.m_height = m_height;

		CreateDedicateTexture();
	}

	D3D11Viewport::D3D11Viewport( DxgiSwapchain<AglType::D3D11>& swapchain )
		: m_width( swapchain.Width() )
		, m_height( swapchain.Height() )
		, m_format( swapchain.Format() )
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
			m_frameBuffer = new D3D11Texture2D( frameBufferTrait, "FrameBuffer", ResourceState::Common, nullptr );
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

				const TextureTrait& trait = m_frameBuffer->GetTrait();
				m_proxy.m_width = trait.m_width;
				m_proxy.m_height = trait.m_height;
			} );
	}
}