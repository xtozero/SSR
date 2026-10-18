#pragma once

#include "PipelineState.h"

namespace agl
{
    class VulkanDepthStencilState final : public DepthStencilState
    {
    public:
        const VkPipelineDepthStencilStateCreateInfo& GetCreateInfo() const
        {
            return m_createInfo;
        }

        explicit VulkanDepthStencilState( const DepthStencilStateDesc& desc );
        VulkanDepthStencilState( const VulkanDepthStencilState& ) = default;
        VulkanDepthStencilState( VulkanDepthStencilState&& ) = default;
        VulkanDepthStencilState& operator=( const VulkanDepthStencilState& ) = default;
        VulkanDepthStencilState& operator=( VulkanDepthStencilState&& ) = default;
        virtual ~VulkanDepthStencilState() override = default;

    private:
        virtual void InitResource() override;
        virtual void FreeResource() override;

        VkPipelineDepthStencilStateCreateInfo m_createInfo;
    };
}
