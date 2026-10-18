#pragma once

#include "PipelineState.h"

namespace agl
{
    class VulkanRasterizerState final : public RasterizerState
    {
    public:
        const VkPipelineRasterizationStateCreateInfo& GetCreateInfo() const
        {
            return m_createInfo;
        }

        explicit VulkanRasterizerState( const RasterizerStateDesc& desc );
        VulkanRasterizerState( const VulkanRasterizerState& ) = default;
        VulkanRasterizerState( VulkanRasterizerState&& ) = default;
        VulkanRasterizerState& operator=( const VulkanRasterizerState& ) = default;
        VulkanRasterizerState& operator=( VulkanRasterizerState&& ) = default;
        virtual ~VulkanRasterizerState() override = default;

    private:
        virtual void InitResource() override;
        virtual void FreeResource() override;

        VkPipelineRasterizationStateCreateInfo m_createInfo;
    };
}
