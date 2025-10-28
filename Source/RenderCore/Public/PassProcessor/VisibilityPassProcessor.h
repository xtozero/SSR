#pragma once

#include "PassProcessor.h"

namespace rendercore
{
    class VisibilityPassProcessor final : public IPassProcessor
    {
    protected:
        virtual std::optional<DrawSnapshot> ProcessInternal(const PrimitiveSubMesh& subMesh, const PassShader& passShader) override;
        virtual PassShader CollectPassShader( MaterialResource& material ) const override;
    };
}