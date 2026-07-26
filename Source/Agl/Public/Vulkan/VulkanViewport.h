#pragma once

#include "Viewport.h"
#include "VulkanTexture.h"

namespace agl
{
    class VulkanSwapchain;

    class VulkanViewport final : public Viewport
    {
    public:
        virtual void Clear() override;
        virtual void Bind( ICommandList& commandList ) const override;

        virtual std::pair<uint32, uint32> Size() const override;
        virtual std::pair<uint32, uint32> SizeOnRenderThread() const override;
        virtual void Resize( uint32 width, uint32 height ) override;
        virtual agl::Texture* Texture() override;

        VulkanViewport( uint32 width, uint32 height, VkFormat format, const float4& bgColor );
        VulkanViewport( VulkanSwapchain& swapchain );

    private:
        virtual void InitResource() override;
        virtual void FreeResource() override;

        void CreateDedicateTexture();

        ViewportProxy m_proxy;

        uint32 m_width;
        uint32 m_height;
        VkFormat m_format;
        float4 m_clearColor;

        uint32 m_bufferIndex = 0;

        RefHandle<VulkanTexture2D> m_frameBuffer;
        RefHandle<VulkanSwapchain> m_swapchain;
    };
}
