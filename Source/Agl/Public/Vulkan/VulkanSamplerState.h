#pragma once

#include "PipelineState.h"

namespace agl
{
    class VulkanSamplerState : public SamplerState
    {
    public:
        explicit VulkanSamplerState( const SamplerStateDesc& desc );

    private:
        virtual void InitResource() override;
        virtual void FreeResource() override;
    };
}
