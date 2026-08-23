#pragma once

#include "PipelineState.h"

namespace agl
{
    class VulkanVertexLayout final : public VertexLayout
    {
    public:

    private:
        virtual void InitResource() override;
        virtual void FreeResource() override;
    };
}
