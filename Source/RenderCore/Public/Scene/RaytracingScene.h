#pragma once

#include <limits>

namespace rendercore
{
    class PrimitiveSceneInfo;
    class RenderGraph;

    class RaytracingScene
    {
    public:
        void Build( RenderGraph& renderGraph );

        void AddInstance( PrimitiveSceneInfo& primitiveSceneInfo, RefHandle<agl::BLAS> blas );
        void RemoveInstance( PrimitiveSceneInfo& primitiveSceneInfo );

        void UpdateInstance( PrimitiveSceneInfo& primitiveSceneInfo );

        RefHandle<agl::TLAS> GetTLAS() const;

        static constexpr uint32 InvalidInstanceId = std::numeric_limits<uint32>::max();

    private:
        void MarkInstanceDirty();

        SparseArray<agl::RaytracingInstanceDesc> m_instances;

        bool m_needsUpdate = false;
        RefHandle<agl::TLAS> m_tlas;
    };
}