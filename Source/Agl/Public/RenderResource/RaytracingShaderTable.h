#pragma once

#include "ShaderResource.h"

#include <span>

namespace agl
{
    struct HitGroupDesc
    {
        size_t GetHash() const;

        IntersectionShader* m_intersection = nullptr;
        AnyHitShader* m_anyHit = nullptr;
        ClosestHitShader* m_closestHit = nullptr;
    };

    struct RaytracingShaderTableDesc
    {
        size_t GetHash() const;

        RayGenerationShader* m_rayGeneration = nullptr;
        std::span<HitGroupDesc> m_hitGroupDescs;
        std::span<MissShader*> m_misses;
    };

    class RaytracingShaderTable : public GraphicsApiResource
    {
    public:
    };
}
