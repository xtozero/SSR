#include "VulkanResourceVeiws.h"

namespace agl
{
    template <typename BaseClass>
    class VulkanImageVeiwBase : public BaseClass
    {
    public:
        GraphicsApiResource* GetOwner() const { return m_owner; }

        explicit VulkanImageVeiwBase( GraphicsApiResource* owner ) noexcept
            : m_owner( owner ) {}

        VulkanImageVeiwBase( const VulkanImageVeiwBase& other )
        {
            *this = other;
        }

        VulkanImageVeiwBase& operator=( const VulkanImageVeiwBase& other )
        {
            if ( this != &other )
            {
                m_owner = other.m_owner;
                m_view = other.m_view;
            }

            return *this;
        }

        VulkanImageVeiwBase( VulkanImageVeiwBase&& other ) noexcept
        {
            *this = std::move( other );
        }

        VulkanImageVeiwBase& operator=( VulkanImageVeiwBase&& other ) noexcept
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

    private:
        GraphicsApiResource* m_owner = nullptr;
        VkImageView m_view = VK_NULL_HANDLE;
    };

    class VulkanShaderResourceView final : public VulkanImageVeiwBase<ShaderResourceView>
    {
        using BaseClass = VulkanImageVeiwBase<ShaderResourceView>;

    public:
        using BaseClass::BaseClass;
        using BaseClass::operator=;

    private:
        void InitResource() override;
        void FreeResource() override;
    };

    class VulkanUnorderedAccessView final : public VulkanImageVeiwBase<UnorderedAccessView>
    {
        using BaseClass = VulkanImageVeiwBase<UnorderedAccessView>;

    public:
        using BaseClass::BaseClass;
        using BaseClass::operator=;

    private:
        void InitResource() override;
        void FreeResource() override;
    };

    class VulkanRenderTargetView final : public VulkanImageVeiwBase<RenderTargetView>
    {
        using BaseClass = VulkanImageVeiwBase<RenderTargetView>;

    public:
        using BaseClass::BaseClass;
        using BaseClass::operator=;

    private:
        void InitResource() override;
        void FreeResource() override;
    };

    class VulkanDepthStencilView final : public VulkanImageVeiwBase<DepthStencilView>
    {
        using BaseClass = VulkanImageVeiwBase<DepthStencilView>;

    public:
        using BaseClass::BaseClass;
        using BaseClass::operator=;

    private:
        void InitResource() override;
        void FreeResource() override;
    };
}
