#include "VulkanTexture.h"

#include "VulkanApi.h"
#include "VulkanResourceVeiws.h"

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
