#include "VulkanSwapchain.h"

#include "VulkanApi.h"

#include <algorithm>
#ifdef _WIN32
#include <vulkan/vulkan_win32.h>
#endif

namespace agl
{
    void VulkanSwapchain::OnBeginFrameRendering()
    {
    }

    void VulkanSwapchain::OnEndFrameRendering()
    {
    }

    DeviceError VulkanSwapchain::Present( bool vSync, bool allowTearing )
    {
        return DeviceError::None;
    }

    void VulkanSwapchain::Clear()
    {
    }

    void* VulkanSwapchain::Handle() const
    {
        return m_windowCtx.m_nativeWindow;
    }

    std::pair<uint32, uint32> VulkanSwapchain::Size() const
    {
        return { m_width, m_height };
    }

    void VulkanSwapchain::Resize( uint32 width, uint32 height )
    {
    }

    agl::Texture* VulkanSwapchain::Texture()
    {
        return nullptr;
    }

    uint32 VulkanSwapchain::GetBackBufferIndex() const
    {
        return m_bufferIndex;
    }

    VulkanSwapchain::VulkanSwapchain( uint32 width, uint32 height, uint32 bufferCount, const engine::PlatformWindowContext& windowCtx, VkFormat format, const float4 clearColor )
        : m_width( width )
        , m_height( height )
        , m_bufferCount( bufferCount )
        , m_windowCtx( windowCtx )
        , m_format( format )
        , m_clearColor{ clearColor[0], clearColor[1], clearColor[2], clearColor[3] }
    {
    }

    VkSurfaceKHR VulkanSwapchain::CreateSurface( VkInstance vkInstance, const engine::PlatformWindowContext& windowCtx )
    {
        VkSurfaceKHR surface = VK_NULL_HANDLE;

#ifdef _WIN32
        VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
            .hinstance = static_cast<HINSTANCE>( windowCtx.m_nativeContext ),
            .hwnd = static_cast<HWND>( windowCtx.m_nativeWindow ),
        };

        VkResult result = vkCreateWin32SurfaceKHR( vkInstance, &surfaceCreateInfo, nullptr, &surface );
        if ( result != VK_SUCCESS )
        {
            surface = VK_NULL_HANDLE;
        }
#endif

        return surface;
    }

    void VulkanSwapchain::InitResource()
    {
        m_surface = CreateSurface( VulkanInstance(), m_windowCtx );
        assert( m_surface != VK_NULL_HANDLE );

        VulkanSwapchainSupportDetails supportDetails = QuerySwapchainSupportDetails();
#ifdef _DEBUG
        AssertSwapchainSupport( supportDetails );
#endif

        VkSwapchainCreateInfoKHR swapchainCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .surface = m_surface,
            .minImageCount = m_bufferCount,
            .imageFormat = m_format,
            .imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
            .imageExtent = VkExtent2D( m_width, m_height ),
            .imageArrayLayers = 1,
            .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            .preTransform = supportDetails.m_capabilities.currentTransform,
            .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            .presentMode = ChooseSwapchainPresentMode( supportDetails ),
            .clipped = VK_TRUE,
            .oldSwapchain = VK_NULL_HANDLE,
        };

        {
            swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            swapchainCreateInfo.queueFamilyIndexCount = 0;
            swapchainCreateInfo.pQueueFamilyIndices = nullptr;
        }

        VkResult result = vkCreateSwapchainKHR( VulkanDevice(), &swapchainCreateInfo, nullptr, &m_swapchain );
        assert( result == VK_SUCCESS );

        uint32 numSwapchainImages = 0;
        vkGetSwapchainImagesKHR( VulkanDevice(), m_swapchain, &numSwapchainImages, nullptr );

        std::vector<VkImage> swapchainImages( numSwapchainImages );
        vkGetSwapchainImagesKHR( VulkanDevice(), m_swapchain, &numSwapchainImages, swapchainImages.data() );

        m_swapchainImageViews.resize( numSwapchainImages );
        for ( uint32 i = 0; i < numSwapchainImages; ++i )
        {
            VkImageViewCreateInfo imageViewCreateInfo = {
                .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                .image = swapchainImages[i],
                .viewType = VK_IMAGE_VIEW_TYPE_2D,
                .format = m_format,
                .components = {
                    .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .a = VK_COMPONENT_SWIZZLE_IDENTITY,
                },
                .subresourceRange = {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .baseMipLevel = 0,
                    .levelCount = 1,
                    .baseArrayLayer = 0,
                    .layerCount = 1,
                }
            };

            result = vkCreateImageView( VulkanDevice(), &imageViewCreateInfo, nullptr, &m_swapchainImageViews[i] );
            assert( result == VK_SUCCESS );
        }
    }

    void VulkanSwapchain::FreeResource()
    {
        for ( VkImageView swapchainImageView : m_swapchainImageViews )
        {
            vkDestroyImageView( VulkanDevice(), swapchainImageView, nullptr );
        }

        if ( m_swapchain != VK_NULL_HANDLE )
        {
            vkDestroySwapchainKHR( VulkanDevice(), m_swapchain, nullptr );
        }

        if ( m_surface != VK_NULL_HANDLE )
        {
            vkDestroySurfaceKHR( VulkanInstance(), m_surface, nullptr );
        }
    }

    VulkanSwapchainSupportDetails VulkanSwapchain::QuerySwapchainSupportDetails() const
    {
        VkPhysicalDevice physicalDevice = VulkanPhysicalDevice();

        VulkanSwapchainSupportDetails details = {};

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR( physicalDevice, m_surface, &details.m_capabilities );

        uint32 numFormats = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR( physicalDevice, m_surface, &numFormats, nullptr );
        if ( numFormats != 0 )
        {
            details.m_formats.resize( numFormats );
            vkGetPhysicalDeviceSurfaceFormatsKHR( physicalDevice, m_surface, &numFormats, details.m_formats.data() );
        }

        uint32 numPresentModes = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR( physicalDevice, m_surface, &numPresentModes, nullptr );
        if ( numPresentModes != 0 )
        {
            details.m_presentModes.resize( numPresentModes );
            vkGetPhysicalDeviceSurfacePresentModesKHR( physicalDevice, m_surface, &numPresentModes, details.m_presentModes.data() );
        }

        return details;
    }

#ifdef _DEBUG
    void VulkanSwapchain::AssertSwapchainSupport( const VulkanSwapchainSupportDetails& details ) const
    {
        if ( ( m_bufferCount < details.m_capabilities.minImageCount )
            || ( m_bufferCount > details.m_capabilities.maxImageCount ) )
        {
            assert( !"Swapchain image count is outside the supported range." );
        }

        if ( ( m_width < details.m_capabilities.minImageExtent.width )
            || ( m_width > details.m_capabilities.maxImageExtent.width ) )
        {
            assert( !"Swapchain width is outside the supported image extent." );
        }

        if ( ( m_height < details.m_capabilities.minImageExtent.height )
            || ( m_height > details.m_capabilities.maxImageExtent.height ) )
        {
            assert( !"Swapchain height is outside the supported image extent." );
        }

        auto surfaceFormatProj = []( VkSurfaceFormatKHR surfaceFormat )
        {
            return surfaceFormat.format;
        };

        auto supportedFormat = std::ranges::find( details.m_formats, m_format, surfaceFormatProj );
        if ( ( supportedFormat == std::end( details.m_formats ) )
            || ( supportedFormat->colorSpace != VK_COLOR_SPACE_SRGB_NONLINEAR_KHR ) )
        {
            assert( !"Requested swapchain surface format or color space is not supported." );
        }
    }
#endif

    VkPresentModeKHR VulkanSwapchain::ChooseSwapchainPresentMode( const VulkanSwapchainSupportDetails& details )
    {
        // ToDo
        for ( VkPresentModeKHR presentMode : details.m_presentModes )
        {
            if ( DefaultAgl::UseVSync() && presentMode == VK_PRESENT_MODE_MAILBOX_KHR )
            {
                return VK_PRESENT_MODE_MAILBOX_KHR;
            }
            else if ( DefaultAgl::AllowTearing() && presentMode == VK_PRESENT_MODE_IMMEDIATE_KHR )
            {
                return VK_PRESENT_MODE_IMMEDIATE_KHR;
            }
        }

        /*
         * This is the only value of presentMode that is required to be supported.
         * https://docs.vulkan.org/refpages/latest/refpages/source/VkPresentModeKHR.html
         */
        return VK_PRESENT_MODE_FIFO_KHR;
    }
}
