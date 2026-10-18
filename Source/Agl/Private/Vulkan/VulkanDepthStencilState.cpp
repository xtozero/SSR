#include "VulkanDepthStencilState.h"

#include "VulkanFlagConverter.h"

using ::agl::DepthStencilStateDesc;
using ::agl::DepthWriteMode;
using ::agl::VulkanFlagConvertor;

namespace
{
    VkPipelineDepthStencilStateCreateInfo ConvertToCreateInfo( const DepthStencilStateDesc& desc )
    {
        return VkPipelineDepthStencilStateCreateInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
            .depthTestEnable = desc.m_depthEnable ? VK_TRUE : VK_FALSE,
            .depthWriteEnable = ( desc.m_depthWriteMode == DepthWriteMode::All ) ? VK_TRUE : VK_FALSE,
            .depthCompareOp = VulkanFlagConvertor::ConvertToCompareOp( desc.m_depthFunc ),
            .depthBoundsTestEnable = VK_FALSE,
            .stencilTestEnable = desc.m_stencilEnable ? VK_TRUE : VK_FALSE,
            .front = {
                .failOp = VulkanFlagConvertor::ConvertToStencilOp( desc.m_frontFace.m_failOp ),
                .passOp = VulkanFlagConvertor::ConvertToStencilOp( desc.m_frontFace.m_passOp ),
                .depthFailOp = VulkanFlagConvertor::ConvertToStencilOp( desc.m_frontFace.m_depthFailOp ),
                .compareOp = VulkanFlagConvertor::ConvertToCompareOp( desc.m_frontFace.m_func ),
                .compareMask = desc.m_stencilReadMask,
                .writeMask = desc.m_stencilWriteMask,
                .reference = 0,
            },
            .back = {
                .failOp = VulkanFlagConvertor::ConvertToStencilOp( desc.m_backFace.m_failOp ),
                .passOp = VulkanFlagConvertor::ConvertToStencilOp( desc.m_backFace.m_passOp ),
                .depthFailOp = VulkanFlagConvertor::ConvertToStencilOp( desc.m_backFace.m_depthFailOp ),
                .compareOp = VulkanFlagConvertor::ConvertToCompareOp( desc.m_backFace.m_func ),
                .compareMask = desc.m_stencilReadMask,
                .writeMask = desc.m_stencilWriteMask,
                .reference = 0,
            },
            .minDepthBounds = 0.f,
            .maxDepthBounds = 1.f,
        };
    }
}

namespace agl
{
    VulkanDepthStencilState::VulkanDepthStencilState( const DepthStencilStateDesc& desc )
        : m_createInfo( ConvertToCreateInfo( desc ) ) {}

    void VulkanDepthStencilState::InitResource()
    {
    }

    void VulkanDepthStencilState::FreeResource()
    {
    }
}
