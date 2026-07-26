#pragma once

#include "Texture.h"

namespace agl
{
    class VulkanTexture : public TextureBase
    {
    public:
        virtual void* Resource() const override;

        virtual void CreateShaderResource( std::optional<ResourceFormat> overrideFormat = {} ) override;
        virtual void CreateUnorderedAccess( std::optional<ResourceFormat> overrideFormat = {} ) override;

    protected:
        virtual void CreateTexture() override;

        VkImage m_image = VK_NULL_HANDLE;
        bool m_isExternalImage = false;

    private:
        virtual void InitResource() override;
        virtual void FreeResource() override;
    };

    class VulkanTexture2D : public VulkanTexture
    {
    public:
        virtual void CreateRenderTarget( std::optional<ResourceFormat> overrideFormat = {} ) override;
        virtual void CreateDepthStencil( std::optional<ResourceFormat> overrideFormat = {} ) override;

        VulkanTexture2D( const TextureDesc& desc, const char* debugName, ResourceState initialState, const ResourceInitData* initData );
        VulkanTexture2D( VkImage image, const char* debugName, const TextureDesc& desc );
    };
}
