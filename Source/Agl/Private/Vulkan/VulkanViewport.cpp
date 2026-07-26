#include "VulkanViewport.h"

namespace agl
{
    void VulkanViewport::Clear()
    {
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
    }
}
