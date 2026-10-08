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
}
