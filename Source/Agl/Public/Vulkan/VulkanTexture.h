#pragma once

#include "Texture.h"

namespace agl
{
    class VulkanTexture : public TextureBase
    {
    public:
        virtual void* Resource() const override;

        virtual void CreateShaderResource( std::optional<ResourceFormat> overrideFormat ) override;
        virtual void CreateUnorderedAccess( std::optional<ResourceFormat> overrideFormat ) override;

    protected:
        virtual void CreateTexture() override;

    private:
        virtual void InitResource() override;
    };

    class VulkanTexture2D : public VulkanTexture
    {
    public:
        virtual void CreateRenderTarget( std::optional<ResourceFormat> overrideFormat ) override;
        virtual void CreateDepthStencil( std::optional<ResourceFormat> overrideFormat ) override;
    };
}
