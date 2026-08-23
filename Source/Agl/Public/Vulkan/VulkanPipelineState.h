#pragma once

#include "PipelineState.h"

namespace agl
{
    class VulkanGraphicsPipelineState final : public GraphicsPipelineState
    {
    public:

    private:
        virtual void InitResource() override;
        virtual void FreeResource() override;
    };

    class VulkanComputePipelineState final : public ComputePipelineState
    {
    public:

    private:
        virtual void InitResource() override;
        virtual void FreeResource() override;
    };
}
