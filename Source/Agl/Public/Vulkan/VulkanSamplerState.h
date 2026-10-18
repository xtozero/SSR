#pragma once

#include "PipelineState.h"

namespace agl
{
    class VulkanSamplerState : public SamplerState
    {
    public:
        const VkSampler& Resource() const;

        explicit VulkanSamplerState( const SamplerStateDesc& desc );
        VulkanSamplerState( const VulkanSamplerState& ) = default;
        VulkanSamplerState( VulkanSamplerState&& ) = default;
        VulkanSamplerState& operator=( const VulkanSamplerState& ) = default;
        VulkanSamplerState& operator=( VulkanSamplerState&& ) = default;
        virtual ~VulkanSamplerState() override = default;

    private:
        virtual void InitResource() override;
        virtual void FreeResource() override;

        VkSamplerCreateInfo m_createInfo = {};

        VkSampler m_sampler = VK_NULL_HANDLE;
    };
}
