#pragma once

#include "PipelineState.h"

namespace agl
{
    class VulkanBlendState final : public BlendState
    {
    public:

    private:
        virtual void InitResource() override;
        virtual void FreeResource() override;
    };
}
