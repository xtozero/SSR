#include "VulkanTexture.h"

#include "VulkanApi.h"

namespace agl
{
    void* VulkanTexture::Resource() const
    {
        return nullptr;
    }

    void VulkanTexture::CreateShaderResource( std::optional<ResourceFormat> overrideFormat )
    {
    }

    void VulkanTexture::CreateUnorderedAccess( std::optional<ResourceFormat> overrideFormat )
    {
    }

    void VulkanTexture::CreateTexture()
    {
    }

    void VulkanTexture::InitResource()
    {
        if ( Resource() == nullptr )
        {
            CreateTexture();
        }

        if ( HasAnyFlags( m_desc.m_miscFlag, ResourceMisc::Intermediate | ResourceMisc::WithoutViews ) )
        {
            return;
        }

        if ( Resource() )
        {
            if ( HasAnyFlags( m_desc.m_bindType, ResourceBindType::ShaderResource ) )
            {
                CreateShaderResource();
            }

            if ( HasAnyFlags( m_desc.m_bindType, ResourceBindType::RandomAccess ) )
            {
                CreateUnorderedAccess();
            }

            if ( HasAnyFlags( m_desc.m_bindType, ResourceBindType::RenderTarget ) )
            {
                CreateRenderTarget();
            }

            if ( HasAnyFlags( m_desc.m_bindType, ResourceBindType::DepthStencil ) )
            {
                CreateDepthStencil();
            }
        }
    }

    void VulkanTexture::FreeResource()
    {
        TextureBase::FreeResource();

        if ( ( m_image != VK_NULL_HANDLE ) && ( m_isExternalImage == false ) )
        {
            vkDestroyImage( VulkanDevice(), m_image, nullptr );
        }
    }

    void VulkanTexture2D::CreateRenderTarget( std::optional<ResourceFormat> overrideFormat )
    {
    }

    void VulkanTexture2D::CreateDepthStencil( std::optional<ResourceFormat> overrideFormat )
    {
    }

    VulkanTexture2D::VulkanTexture2D( const TextureDesc& desc, const char* debugName, ResourceState initialState, const ResourceInitData* initData )
    {
    }

    VulkanTexture2D::VulkanTexture2D( VkImage image, const char* debugName, const TextureDesc& desc )
    {
        if ( image == VK_NULL_HANDLE )
        {
            return;
        }

        m_image = image;
        m_isExternalImage = true;

        /*
        VkImageViewCreateInfo imageViewCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = image,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = ConvertFormatToVkFormat( desc.m_format ),
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

        VkResult result = vkCreateImageView( VulkanDevice(), &imageViewCreateInfo, nullptr, &m_imageView );
        assert( result == VK_SUCCESS );
        */

        Rename( Name( debugName ) );
        m_desc = desc;
    }
}
