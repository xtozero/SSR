#include "VulkanTexture.h"

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
    }

    void VulkanTexture2D::CreateRenderTarget( std::optional<ResourceFormat> overrideFormat )
    {
    }

    void VulkanTexture2D::CreateDepthStencil( std::optional<ResourceFormat> overrideFormat )
    {
    }
}
