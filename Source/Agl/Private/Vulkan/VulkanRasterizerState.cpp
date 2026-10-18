#include "VulkanRasterizerState.h"

#include "VulkanFlagConverter.h"

using ::agl::RasterizerStateDesc;
using ::agl::VulkanFlagConvertor;

namespace
{
    VkPipelineRasterizationStateCreateInfo ConvertToCreateInfo( const RasterizerStateDesc& desc )
    {
        bool depthBiasEnable = ( desc.m_depthBias != 0 ) || ( desc.m_slopeScaleDepthBias != 0.f );

        // TODO : Review Vulkan mappings for m_depthClipEnable, m_scissorEnable, m_multisampleEnable, and m_antialiasedLineEnable.
        return VkPipelineRasterizationStateCreateInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .depthClampEnable = VK_FALSE,
            .rasterizerDiscardEnable = VK_FALSE,
            .polygonMode = VulkanFlagConvertor::ConvertToPolygonMode( desc.m_fillMode ),
            .cullMode = VulkanFlagConvertor::ConvertToCullMode( desc.m_cullMode ),
            .frontFace = desc.m_frontCounterClockwise ? VK_FRONT_FACE_COUNTER_CLOCKWISE : VK_FRONT_FACE_CLOCKWISE,
            .depthBiasEnable = depthBiasEnable,
            .depthBiasConstantFactor = static_cast<float>( desc.m_depthBias ),
            .depthBiasClamp = desc.m_depthBiasClamp,
            .depthBiasSlopeFactor = desc.m_slopeScaleDepthBias,
            .lineWidth = 1.f,
        };
    }
}

namespace agl
{
    VulkanRasterizerState::VulkanRasterizerState( const RasterizerStateDesc& desc )
        : m_createInfo( ConvertToCreateInfo( desc ) ) {}

    void VulkanRasterizerState::InitResource()
    {
    }

    void VulkanRasterizerState::FreeResource()
    {
    }
}
