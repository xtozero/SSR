#pragma once

#include "PipelineState.h"

namespace agl
{
    class VulkanDepthStencilState final : public DepthStencilState
    {
    public:

    private:
        virtual void InitResource() override;
        virtual void FreeResource() override;
    };
}
