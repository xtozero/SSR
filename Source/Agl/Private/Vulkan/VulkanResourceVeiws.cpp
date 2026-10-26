#include "VulkanResourceVeiws.h"

namespace agl
{
    VulkanImageRenderTargetView::VulkanImageRenderTargetView( VulkanTexture* owner, const ColorF& clearColor ) noexcept
        : BaseClass( owner )
        , m_clearColor( clearColor )
    {}

    ColorF VulkanImageRenderTargetView::GetClearColor() const
    {
        return m_clearColor;
    }

    VulkanImageDepthStencilView::VulkanImageDepthStencilView( VulkanTexture* owner, float depthValue, uint8 stencilValue ) noexcept
        : BaseClass( owner )
        , m_depthClearValue( depthValue )
        , m_stencilClearValue( stencilValue )
    {
    }

    float VulkanImageDepthStencilView::GetDepthClearValue() const
    {
        return m_depthClearValue;
    }

    uint8 VulkanImageDepthStencilView::GetStencilClearValue() const
    {
        return m_stencilClearValue;
    }
}
