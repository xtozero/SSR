#pragma once

#include "PipelineState.h"

namespace agl
{
    class VulkanBlendState final : public BlendState
    {
    public:
        const VkPipelineColorBlendStateCreateInfo& GetCreateInfo() const
        {
            return m_createInfo;
        }

        const float* GetBlendFactor() const;

        void SetBlendFactor( const float( &blendFactor )[4] );

        explicit VulkanBlendState( const BlendStateDesc& desc );
        VulkanBlendState( const VulkanBlendState& ) = default;
        VulkanBlendState( VulkanBlendState&& ) = default;
        VulkanBlendState& operator=( const VulkanBlendState& ) = default;
        VulkanBlendState& operator=( VulkanBlendState&& ) = default;
        virtual ~VulkanBlendState() override = default;

    private:
        virtual void InitResource() override;
        virtual void FreeResource() override;

        void InitializeCreateInfo( const BlendStateDesc& desc );

        VkPipelineColorBlendStateCreateInfo m_createInfo;
        VkPipelineColorBlendAttachmentState m_attachmentState[MaxRendertagets];
    };
}
