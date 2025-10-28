#pragma once

#include "ShaderBindings.h"

namespace rendercore
{
    class ShadingSnapshot final
    {
    public:
        agl::ShaderBindings m_shaderBindings;

        ComputeShader* m_computeShader = nullptr;
        RefHandle<agl::ComputePipelineState> m_pso;

        uint32 m_startIndexLocation = 0;
        uint32 m_baseVertexLocation = 0;
    };

    struct ShadingSnapshotDynamicInstancingEqual final
    {
        bool operator()( const ShadingSnapshot& lhs, const ShadingSnapshot& rhs ) const
        {
            return lhs.m_shaderBindings.MatchsForDynamicInstancing( rhs.m_shaderBindings )
                && lhs.m_computeShader == rhs.m_computeShader
                && lhs.m_startIndexLocation == rhs.m_startIndexLocation
                && lhs.m_baseVertexLocation == rhs.m_baseVertexLocation;
        }
    };

    struct ShadingSnapshotDynamicInstancingHasher final
    {
        size_t operator()( const ShadingSnapshot& snapshot ) const
        {
            static size_t typeHash = typeid( ShadingSnapshotDynamicInstancingHasher ).hash_code();
            size_t hash = typeHash;

            HashCombine( hash, snapshot.m_shaderBindings.HashForDynamicInstaning() );
            HashCombine( hash, snapshot.m_computeShader );
            HashCombine( hash, snapshot.m_startIndexLocation );
            HashCombine( hash, snapshot.m_baseVertexLocation );

            return hash;
        }
    };

    class ShadingSnapshotBucket final
    {
    public:
        int32 Add( const ShadingSnapshot& snapshot );
        void Remove( int32 id );

        ShadingSnapshot& Get( int32 id );

    private:
        struct SharedSnapshotId
        {
            constexpr explicit SharedSnapshotId( int32 id ) : m_id( id ) {}

            int32 m_id = -1;
            int32 m_ref = 0;
        };

        std::unordered_map<ShadingSnapshot, SharedSnapshotId, ShadingSnapshotDynamicInstancingHasher, ShadingSnapshotDynamicInstancingEqual> m_bucket;
        SparseArray<ShadingSnapshot> m_snapshots;
    };

    struct VisibleShadingSnapshot
    {
        uint32 m_primitiveId = 0;
        ShadingSnapshot* m_shadingSnapshot = nullptr;

        friend std::strong_ordering operator<=>( const VisibleShadingSnapshot& lhs, const VisibleShadingSnapshot& rhs )
        {
            return std::tie( lhs.m_primitiveId, lhs.m_shadingSnapshot ) <=> std::tie( rhs.m_primitiveId, rhs.m_shadingSnapshot );
        }
    };
}
