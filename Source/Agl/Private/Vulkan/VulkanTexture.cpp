#include "VulkanTexture.h"

#include "VulkanApi.h"
#include "VulkanResourceVeiws.h"

using ::agl::ResourceBindType;
using ::agl::ResourceMisc;
using ::agl::TextureDesc;

namespace
{
    VkImageCreateFlags ConvertToVkImageCreateFlags( const TextureDesc& desc )
    {
        if ( HasAnyFlags( desc.m_miscFlag, ResourceMisc::TextureCube ) )
        {
            return VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
        }

        return 0;
    }

    VkImageType ConvertToVkImageType( const TextureDesc& desc )
    {
        if ( HasAnyFlags( desc.m_miscFlag, ResourceMisc::Texture3D ) )
        {
            return VK_IMAGE_TYPE_3D;
        }
        else
        {
            return VK_IMAGE_TYPE_2D;
        }
    }

    VkImageUsageFlags ConvertToVkImageUsage( const TextureDesc& desc )
    {
        // bool bDownload = HasAnyFlags( m_desc.m_access, ResourceAccess::CpuRead ); // TODO maybe
        VkImageUsageFlags usage = 0;

        if ( HasAnyFlags( desc.m_bindType, ResourceBindType::CopySource ) )
        {
            usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        }

        if ( HasAnyFlags( desc.m_bindType, ResourceBindType::CopyDest ) )
        {
            usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        }

        if ( HasAnyFlags( desc.m_bindType, ResourceBindType::ShaderResource ) )
        {
            usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
        }

        if ( HasAnyFlags( desc.m_bindType, ResourceBindType::RenderTarget ) )
        {
            usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        }

        if ( HasAnyFlags( desc.m_bindType, ResourceBindType::DepthStencil ) )
        {
            usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        }

        if ( HasAnyFlags( desc.m_bindType, ResourceBindType::RandomAccess ) )
        {
            usage |= VK_IMAGE_USAGE_STORAGE_BIT;
        }

        return usage;
    }
}

namespace agl
{
    void* VulkanTexture::Resource() const
    {
        return m_image;
    }

    void VulkanTexture::CreateShaderResource( std::optional<ResourceFormat> overrideFormat )
    {
        m_srv = new VulkanImageShaderResourceView( this );
        m_srv->Init();
    }

    void VulkanTexture::CreateUnorderedAccess( std::optional<ResourceFormat> overrideFormat )
    {
        m_uav.resize( m_desc.m_mipLevels );
        for ( uint32 mipSlice = 0; mipSlice < m_desc.m_mipLevels; ++mipSlice )
        {
            m_uav[mipSlice] = new VulkanImageUnorderedAccessView( this );
            m_uav[mipSlice]->Init();
        }
    }

    VulkanTexture::VulkanTexture( const TextureDesc& desc, const char* debugName, ResourceState initialState, const ResourceInitData* initData )
        : TextureBase( desc, debugName, initialState, initData ) {}

    void VulkanTexture::CreateTexture()
    {
        VkImageType vkImageType = ConvertToVkImageType( m_desc );
        bool isTexture3D = vkImageType == VK_IMAGE_TYPE_3D;

        VkImageCreateInfo imageCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .flags = ConvertToVkImageCreateFlags( m_desc ),
            .imageType = vkImageType,
            .format = ConvertToVkFormat( m_desc.m_format ),
            .extent = {
                .width = m_desc.m_width,
                .height = m_desc.m_height,
                .depth = isTexture3D ? m_desc.m_depth : 1,
            },
            .mipLevels = m_desc.m_mipLevels,
            .arrayLayers = isTexture3D ? 1 : m_desc.m_depth,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .tiling = VK_IMAGE_TILING_OPTIMAL,
            .usage = ConvertToVkImageUsage( m_desc ),
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        };

        VkResult result = vkCreateImage( VulkanDevice(), &imageCreateInfo, nullptr, &m_image );
        assert( result == VK_SUCCESS );

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements( VulkanDevice(), m_image, &memRequirements );

        VkMemoryAllocateInfo allocInfo = {
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .allocationSize = memRequirements.size,
            .memoryTypeIndex = FindVulkanMemoryType( memRequirements.memoryTypeBits, ConvertToVkMemoryPropertyFlags( m_desc.m_access ) ),
        };

        result = vkAllocateMemory( VulkanDevice(), &allocInfo, nullptr, &m_deviceMemory );
        assert( result == VK_SUCCESS );

        VkBindImageMemoryInfo bindInfo = {
            .sType = VK_STRUCTURE_TYPE_BIND_IMAGE_MEMORY_INFO,
            .image = m_image,
            .memory = m_deviceMemory,
            .memoryOffset = 0,
        };

        result = vkBindImageMemory2( VulkanDevice(), 1, &bindInfo );
        assert( result == VK_SUCCESS );
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
            m_image = VK_NULL_HANDLE;
        }

        if ( m_deviceMemory != VK_NULL_HANDLE )
        {
            vkFreeMemory( VulkanDevice(), m_deviceMemory, nullptr );
            m_deviceMemory = VK_NULL_HANDLE;
        }
    }

    void VulkanTexture2D::CreateRenderTarget( std::optional<ResourceFormat> overrideFormat )
    {
        ColorF clearColor = ColorF::Black;
        if ( m_desc.m_clearValue )
        {
            clearColor = m_desc.m_clearValue->m_color;
        }

        m_rtv = new VulkanImageRenderTargetView( this, clearColor );
        m_rtv->Init();
    }

    void VulkanTexture2D::CreateDepthStencil( std::optional<ResourceFormat> overrideFormat )
    {
        m_dsv = new VulkanImageDepthStencilView( this );
        m_dsv->Init();
    }

    VulkanTexture2D::VulkanTexture2D( const TextureDesc& desc, const char* debugName, ResourceState initialState, const ResourceInitData* initData )
        : VulkanTexture( desc, debugName, initialState, initData ) {}

    VulkanTexture2D::VulkanTexture2D( VkImage image, const char* debugName, const TextureDesc& desc )
    {
        if ( image == VK_NULL_HANDLE )
        {
            return;
        }

        m_image = image;
        m_isExternalImage = true;

        Rename( Name( debugName ) );
        m_desc = desc;
    }
}
