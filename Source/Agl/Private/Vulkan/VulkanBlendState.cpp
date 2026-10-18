#include "VulkanBlendState.h"

using ::agl::BlendStateDesc;

namespace agl
{
    const float* VulkanBlendState::GetBlendFactor() const
    {
        return m_createInfo.blendConstants;
    }

    void VulkanBlendState::SetBlendFactor( const float(& blendFactor)[4] )
    {
        for ( uint32 i = 0; i < 4; ++i )
        {
            m_createInfo.blendConstants[i] = blendFactor[i];
        }
    }

    VulkanBlendState::VulkanBlendState( const BlendStateDesc& desc )
    {
        InitializeCreateInfo( desc );
    }

    void VulkanBlendState::InitResource()
    {
    }

    void VulkanBlendState::FreeResource()
    {
    }

    void VulkanBlendState::InitializeCreateInfo( const BlendStateDesc& desc )
    {
        m_createInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .logicOpEnable = VK_FALSE,
            .logicOp = VK_LOGIC_OP_NO_OP,
            .attachmentCount = MaxRendertagets, // TODO
            .pAttachments = m_attachmentState,
            .blendConstants = { 0.f, 0.f, 0.f, 0.f },
        };

        for ( uint32 i = 0; i < MaxRendertagets; ++i )
        {
            const RenderTargetBlendDesc& rtBlend = desc.m_renderTarget[i];

            m_attachmentState[i] = {
                .blendEnable = rtBlend.m_blendEnable,
                .srcColorBlendFactor = VulkanFlagConvertor::ConvertToBlend( rtBlend.m_srcBlend ),
                .dstColorBlendFactor = VulkanFlagConvertor::ConvertToBlend( rtBlend.m_destBlend ),
                .colorBlendOp = VulkanFlagConvertor::ConvertToBlendOp( rtBlend.m_blendOp ),
                .srcAlphaBlendFactor = VulkanFlagConvertor::ConvertToBlend( rtBlend.m_srcBlendAlpha ),
                .dstAlphaBlendFactor = VulkanFlagConvertor::ConvertToBlend( rtBlend.m_destBlendAlpha ),
                .alphaBlendOp = VulkanFlagConvertor::ConvertToBlendOp( rtBlend.m_blendOpAlpha ),
                .colorWriteMask = VulkanFlagConvertor::ConvertToColorComponent( rtBlend.m_renderTargetWriteMask ),
            };
        }
    }
}
