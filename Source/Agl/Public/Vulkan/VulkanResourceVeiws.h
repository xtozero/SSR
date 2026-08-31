#pragma once

#include "VulkanApi.h"
#include "VulkanTexture.h"

namespace agl
{
    class VulkanBuffer;
    class VulkanTexture;

    template <typename BaseClass>
    class VulkanImageViewBase : public BaseClass
    {
    public:
        VulkanTexture* GetOwner() const { return m_owner; }
        VkImageView GetImageView() const { return m_view; }

        explicit VulkanImageViewBase( VulkanTexture* owner ) noexcept
            : m_owner( owner ) {}

        VulkanImageViewBase( const VulkanImageViewBase& other )
        {
            *this = other;
        }

        VulkanImageViewBase& operator=( const VulkanImageViewBase& other )
        {
            if ( this != &other )
            {
                m_owner = other.m_owner;
                m_view = other.m_view;
            }

            return *this;
        }

        VulkanImageViewBase( VulkanImageViewBase&& other ) noexcept
        {
            *this = std::move( other );
        }

        VulkanImageViewBase& operator=( VulkanImageViewBase&& other ) noexcept
        {
            if ( this != &other )
            {
                m_owner = other.m_owner;
                m_view = other.m_view;

                other.m_owner = nullptr;
                other.m_view = VK_NULL_HANDLE;
            }

            return *this;
        }

    protected:
        VulkanTexture* m_owner = nullptr;
        VkImageView m_view = VK_NULL_HANDLE;

    private:
        virtual void InitResource() override
        {
            const TextureDesc& desc = m_owner->GetDesc();

            VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_3D;
            if ( IsTexture2D( desc ) )
            {
                if ( HasAnyFlags( desc.m_miscFlag, ResourceMisc::TextureCube ) )
                {
                    viewType = ( desc.m_depth == 6 ) ? VK_IMAGE_VIEW_TYPE_CUBE : VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
                }
                else
                {
                    viewType = ( desc.m_depth == 1 ) ? VK_IMAGE_VIEW_TYPE_2D : VK_IMAGE_VIEW_TYPE_2D_ARRAY;
                }
            }

            VkImageAspectFlags aspectMask = HasAnyFlags( desc.m_bindType, ResourceBindType::DepthStencil )
                                                ? (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT)
                                                : VK_IMAGE_ASPECT_COLOR_BIT;

            VkImageViewCreateInfo imageViewCreateInfo = {
                .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                .image = static_cast<VkImage>( m_owner->Resource() ),
                .viewType = viewType,
                .format = ConvertToVkFormat( desc.m_format ),
                .components = {
                    .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .a = VK_COMPONENT_SWIZZLE_IDENTITY,
                },
                .subresourceRange = {
                    .aspectMask = aspectMask,
                    .baseMipLevel = 0,
                    .levelCount = desc.m_mipLevels,
                    .baseArrayLayer = 0,
                    .layerCount = desc.m_depth,
                }
            };

            VkResult result = vkCreateImageView( VulkanDevice(), &imageViewCreateInfo, nullptr, &m_view );
            assert( result == VK_SUCCESS );
        }

        virtual void FreeResource() override
        {
            if ( m_view != VK_NULL_HANDLE )
            {
                vkDestroyImageView( VulkanDevice(), m_view, nullptr );
                m_view = VK_NULL_HANDLE;
            }
        }
    };

    class VulkanImageShaderResourceView final : public VulkanImageViewBase<ShaderResourceView>
    {
        using BaseClass = VulkanImageViewBase<ShaderResourceView>;

    public:
        using BaseClass::VulkanImageViewBase;
        using BaseClass::operator=;
    };

    class VulkanImageUnorderedAccessView final : public VulkanImageViewBase<UnorderedAccessView>
    {
        using BaseClass = VulkanImageViewBase<UnorderedAccessView>;

    public:
        using BaseClass::VulkanImageViewBase;
        using BaseClass::operator=;
    };

    class VulkanImageRenderTargetView final : public VulkanImageViewBase<RenderTargetView>
    {
        using BaseClass = VulkanImageViewBase<RenderTargetView>;

    public:
        using BaseClass::operator=;

        VulkanImageRenderTargetView( VulkanTexture* owner, const ColorF& clearColor ) noexcept;

        ColorF GetClearColor() const;

    private:
        ColorF m_clearColor = ColorF::Black;
    };

    class VulkanImageDepthStencilView final : public VulkanImageViewBase<DepthStencilView>
    {
        using BaseClass = VulkanImageViewBase<DepthStencilView>;

    public:
        using BaseClass::VulkanImageViewBase;
        using BaseClass::operator=;
    };

    template <typename BaseClass>
    class VulkanBufferViewBase : public BaseClass
    {
    public:
        GraphicsApiResource* GetOwner() const { return m_owner; }

        explicit VulkanBufferViewBase( VulkanBuffer* owner ) noexcept
            : m_owner( owner ) {}

        VulkanBufferViewBase( const VulkanBufferViewBase& other )
        {
            *this = other;
        }

        VulkanBufferViewBase& operator=( const VulkanBufferViewBase& other )
        {
            if ( this != &other )
            {
                m_owner = other.m_owner;
                m_view = other.m_view;
            }

            return *this;
        }

        VulkanBufferViewBase( VulkanBufferViewBase&& other ) noexcept
        {
            *this = std::move( other );
        }

        VulkanBufferViewBase& operator=( VulkanBufferViewBase&& other ) noexcept
        {
            if ( this != &other )
            {
                m_owner = other.m_owner;
                m_view = other.m_view;

                other.m_owner = nullptr;
                other.m_view = VK_NULL_HANDLE;
            }

            return *this;
        }

    protected:
        VulkanBuffer* m_owner = nullptr;
        VkBufferView m_view = VK_NULL_HANDLE;

    private:
        virtual void InitResource() override
        {

        }

        virtual void FreeResource() override
        {
            if ( m_view != VK_NULL_HANDLE )
            {
                vkDestroyBufferView( VulkanDevice(), m_view, nullptr );
                m_view = VK_NULL_HANDLE;
            }
        }
    };

    class VulkanBufferShaderResourceView final : public VulkanBufferViewBase<ShaderResourceView>
    {
        using BaseClass = VulkanBufferViewBase<ShaderResourceView>;

    public:
        using BaseClass::VulkanBufferViewBase;
        using BaseClass::operator=;
    };

    class VulkanBufferUnorderedAccessView final : public VulkanBufferViewBase<UnorderedAccessView>
    {
        using BaseClass = VulkanBufferViewBase<UnorderedAccessView>;

    public:
        using BaseClass::VulkanBufferViewBase;
        using BaseClass::operator=;
    };
}
