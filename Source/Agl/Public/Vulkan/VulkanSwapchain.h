#pragma once

#include "Canvas.h"

#include <vulkan/vulkan.h>

namespace agl
{
    struct VulkanSwapchainSupportDetails
    {
        VkSurfaceCapabilitiesKHR m_capabilities;
        std::vector<VkSurfaceFormatKHR> m_formats;
        std::vector<VkPresentModeKHR> m_presentModes;
    };

    class VulkanSwapchain final : public Canvas
    {
    public:
        virtual void OnBeginFrameRendering() override;
        virtual void OnEndFrameRendering() override;

        virtual DeviceError Present( bool vSync, bool allowTearing ) override;
        virtual void Clear() override;

        virtual void* Handle() const override;
        virtual std::pair<uint32, uint32> Size() const override;
        virtual void Resize( uint32 width, uint32 height ) override;

        virtual agl::Texture* Texture() override;

        virtual uint32 GetBackBufferIndex() const override;

        uint32 Width() const
        {
            return m_width;
        }

        uint32 Height() const
        {
            return m_height;
        }

        VkFormat Format() const
        {
            return m_format;
        }

        VulkanSwapchain( uint32 width, uint32 height, uint32 bufferCount, const engine::PlatformWindowContext& windowCtx, VkFormat format, const float4 clearColor );

        static VkSurfaceKHR CreateSurface( VkInstance vkInstance, const engine::PlatformWindowContext& windowCtx );

    private:
        virtual void InitResource() override;
        virtual void FreeResource() override;

        VulkanSwapchainSupportDetails QuerySwapchainSupportDetails() const;
#ifdef _DEBUG
        void AssertSwapchainSupport( const VulkanSwapchainSupportDetails& details ) const;
#endif
        static VkPresentModeKHR ChooseSwapchainPresentMode( const VulkanSwapchainSupportDetails& details );

        uint32 m_width = 0;
        uint32 m_height = 0;
        uint32 m_bufferCount = 0;
        engine::PlatformWindowContext m_windowCtx;
        VkFormat m_format = VK_FORMAT_UNDEFINED;
        float4 m_clearColor;

        VkSurfaceKHR m_surface = VK_NULL_HANDLE;
        VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;

        uint32 m_bufferIndex = 0;
        std::vector<RefHandle<agl::Texture>, InlineAllocator<RefHandle<agl::Texture>, 2>> m_backBuffers;
    };
}
