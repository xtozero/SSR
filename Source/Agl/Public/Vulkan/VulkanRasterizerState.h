#pragma once

#include "PipelineState.h"

namespace agl
{
    class VulkanRasterizerState final : public RasterizerState
    {
    public:

    private:
        void InitResource() override;
        void FreeResource() override;
    };
}
