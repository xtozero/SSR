#include "Viewport.h"

#include "../RenderResource/Viewport.h"
#include "AbstractGraphicsInterface.h"
#include "Canvas.h"
#include "InterfaceFactories.h"
#include "IRenderResourceManager.h"
#include "TaskScheduler.h"

namespace rendercore
{
	void HitProxyMap::Init( uint32 width, uint32 height )
	{
		if ( ( m_width == width ) && ( m_height = height ) )
		{
			return;
		}

		m_width = width;
		m_height = height;

		agl::TextureTrait hitProxyTexTrait = {
			.m_width = width,
			.m_height = height,
			.m_depth = 1,
			.m_sampleCount = 1,
			.m_sampleQuality = 0,
			.m_mipLevels = 1,
			.m_format = agl::ResourceFormat::R8G8B8A8_UNORM,
			.m_access = agl::ResourceAccessFlag::Default,
			.m_bindType = agl::ResourceBindType::RenderTarget,
			.m_miscFlag = agl::ResourceMisc::None,
			.m_clearValue = agl::ResourceClearValue{
				.m_format = agl::ResourceFormat::R8G8B8A8_UNORM,
				.m_color = { 1.f, 1.f, 1.f, 1.f }
			}
		};

		m_hitProxyTexture = agl::Texture::Create( hitProxyTexTrait, "HitProxy.Texture" );
		EnqueueRenderTask(
			[this]()
			{
				m_hitProxyTexture->Init();
			} );

		agl::TextureTrait hitProxyCpuTexTrait = {
			.m_width = width,
			.m_height = height,
			.m_depth = 1,
			.m_sampleCount = 1,
			.m_sampleQuality = 0,
			.m_mipLevels = 1,
			.m_format = agl::ResourceFormat::R8G8B8A8_UNORM,
			.m_access = agl::ResourceAccessFlag::Download,
			.m_bindType = agl::ResourceBindType::None,
			.m_miscFlag = agl::ResourceMisc::None
		};

		m_hitProxyCpuTexture = agl::Texture::Create( hitProxyCpuTexTrait, "HitProxy.CpuTexture" );
		EnqueueRenderTask(
			[this]()
			{
				m_hitProxyCpuTexture->Init();
			} );
	}

	agl::Texture* HitProxyMap::Texture() const
	{
		return m_hitProxyTexture.Get();
	}

	agl::Texture* HitProxyMap::CpuTexture() const
	{
		return m_hitProxyCpuTexture.Get();
	}

	void Viewport::Clear()
	{
		if ( m_pViewport.Get() )
		{
			m_pViewport->Clear( m_clearColor.RGBA() );
		}
	}

	void Viewport::Bind( agl::ICommandListBase& commandList ) const
	{
		if ( m_pViewport.Get() )
		{
			m_pViewport->Bind( commandList );
		}
	}

	std::pair<uint32, uint32> Viewport::Size() const
	{
		if ( m_pViewport.Get() )
		{
			return m_pViewport->Size();
		}

		return {};
	}

	RENDERCORE_DLL std::pair<uint32, uint32> Viewport::SizeOnRenderThread() const
	{
		if ( m_pViewport.Get() )
		{
			return m_pViewport->SizeOnRenderThread();
		}

		return {};
	}

	void Viewport::Resize( const std::pair<uint32, uint32>& newSize )
	{
		if ( m_pViewport.Get() )
		{
			m_pViewport->Resize( newSize );
		}
	}

	agl::Texture* Viewport::Texture()
	{
		if ( m_pViewport.Get() )
		{
			return m_pViewport->Texture();
		}

		return nullptr;
	}

	HitProxyMap& Viewport::GetHitPorxyMap()
	{
		return m_hitProxyMap;
	}

	Viewport::Viewport( uint32 width, uint32 height, agl::ResourceFormat format, const float4& bgColor )
		: m_clearColor( bgColor )
	{
		m_pViewport = agl::Viewport::Create( width, height, format, bgColor );
		EnqueueRenderTask(
			[viewport = m_pViewport]()
			{
				viewport->Init();
			} );
	}

	Viewport::Viewport( rendercore::Canvas& canvas )
	{
		m_pViewport = agl::Viewport::Create( *canvas.Resource() );
		EnqueueRenderTask(
			[viewport = m_pViewport]()
			{
				viewport->Init();
			} );
	}

	Viewport::~Viewport()
	{
	}
}
