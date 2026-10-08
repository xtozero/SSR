#include "VulkanViewport.h"

namespace agl
{
    void VulkanViewport::Clear()
    {
        agl::Texture* backBuffer = m_frameBuffer.Get();
        if ( backBuffer == nullptr )
        {
            return;
        }

        ResourceTransition transition
        {
            .m_pResource = backBuffer->Resource(),
            .m_pTransitionable = backBuffer,
            .m_subResource = AllSubResource,
            .m_state = ResourceState::RenderTarget,
            .m_isBuffer = false,
        };

        ICommandList* commandList = GetInterface<IAgl>()->GetCommandList();
        commandList->AddTransition( transition );

        if ( m_frameBuffer.Get() != nullptr )
        {
            if ( RenderTargetView* rtv = m_frameBuffer->RTV() )
            {
                commandList->ClearRenderTarget( rtv );
            }
        }
    }

    void VulkanViewport::Bind( ICommandList& commandList ) const
    {
    }

    std::pair<uint32, uint32> VulkanViewport::Size() const
    {
        return { m_width, m_height };
    }

    std::pair<uint32, uint32> VulkanViewport::SizeOnRenderThread() const
    {
        assert( IsInRenderThread() );
        return { m_proxy.m_width, m_proxy.m_height };
    }

    void VulkanViewport::Resize( uint32 width, uint32 height )
    {
    }

    agl::Texture* VulkanViewport::Texture()
    {
        return ( m_swapchain.Get() != nullptr )
            ? m_swapchain->Texture()
            : m_frameBuffer.Get();
    }

    VulkanViewport::VulkanViewport( uint32 width, uint32 height, VkFormat format, const float4& bgColor )
        : m_width( width )
        , m_height( height )
        , m_format( format )
        , m_clearColor{ bgColor[0], bgColor[1], bgColor[2], bgColor[3] }
    {
        m_proxy.m_width = m_width;
        m_proxy.m_height = m_height;

        CreateDedicateTexture();
    }

    VulkanViewport::VulkanViewport( VulkanSwapchain& swapchain )
        : m_width( swapchain.Width() )
        , m_height( swapchain.Height() )
        , m_format( swapchain.Format() )
        , m_clearColor{}
        , m_swapchain( &swapchain )
    {
        m_proxy.m_width = m_width;
        m_proxy.m_height = m_height;
    }

    void VulkanViewport::InitResource()
    {
    }

    void VulkanViewport::FreeResource()
    {
        m_frameBuffer = nullptr;
        m_swapchain = nullptr;
    }

    void VulkanViewport::CreateDedicateTexture()
    {
        ResourceFormat orignalFormat = ConvertVkFormatToFormat( m_format );

        TextureDesc frameBufferDesc = {
            .m_width = m_width,
            .m_height = m_height,
            .m_depth = 1,
            .m_sampleCount = 1,
            .m_sampleQuality = 0,
            .m_mipLevels = 1,
            .m_format = orignalFormat,
            .m_access = ResourceAccess::Default,
            .m_bindType = ResourceBindType::RenderTarget | ResourceBindType::ShaderResource,
            .m_miscFlag = ResourceMisc::WithoutViews,
            .m_clearValue = ResourceClearValue{
                .m_format = orignalFormat,
                .m_color = { m_clearColor[0], m_clearColor[1], m_clearColor[2], m_clearColor[3] }
            }
        };

        if ( m_frameBuffer == nullptr )
        {
            m_frameBuffer = new VulkanTexture2D( frameBufferDesc, "FrameBuffer", ResourceState::Common, nullptr);
        }
        /*
        else
        {
            m_frameBuffer->Reconstruct( frameBufferDesc, nullptr );
        }
        */

        EnqueueRenderTask(
            [this, orignalFormat]()
            {
                GetInterface<IAgl>()->WaitGPU();

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
