#pragma once

#include "DrawSnapshot.h"
#include "ShadingSnapshot.h"
#include "RenderGraphResource.h"

namespace rendercore
{
    class IScene;

    struct VisibilityPassData
    {
        RenderFrameArray<VisibleDrawSnapshot> m_visibilityPassSnapshots;
        RenderFrameArray<uint32> m_shadingSnapshotIds;
        RenderFrameArray<VisibleShadingSnapshot> m_shadingSnapshots;

        uint32 m_maxShadingSnapshotId = 0;
    };

    struct VisibilityBuffer
    {
        static constexpr uint32 PrefixSumBlockSize = 256;

        struct RenderBufferParam
        {
            RenderViewGroup& m_renderViewGroup;

            ResourceBinder& m_resourceBinder;

            VisibilityPassData& m_visibilityPassData;

            RenderGraphTexture* m_visibility = nullptr;
            agl::Texture* m_depthStencil = nullptr;

            bool IsValid() const
            {
                return m_visibility != nullptr
                    && m_depthStencil != nullptr;
            }
        };

        static void RenderBuffer( RenderGraph& renderGraph, const RenderBufferParam& param );

        static RefHandle<agl::Buffer> UploadPrimitiveIds( const VisibilityPassData& passData );

        struct CountDrawCallIdParam
        {
            uint32 m_numDrawCallIds = 0;
            RenderGraphTexture* m_visibility = nullptr;

            bool IsValid() const
            {
                return m_numDrawCallIds > 0
                    && m_visibility != nullptr;
            }
        };

        struct CountDrawCallIdOutput
        {
            RenderGraphBuffer* m_counter = nullptr;
            RenderGraphBuffer* m_indirectArgs = nullptr;
        };

        [[nodiscard]] static CountDrawCallIdOutput CountDrawCallId( RenderGraph& renderGraph, const CountDrawCallIdParam& param );

        struct CalcConterPrefixSumParam
        {
            uint32 m_numDrawCallIds = 0;
            RenderGraphBuffer* m_counter = nullptr;

            bool IsValid() const
            {
                return m_numDrawCallIds > 0
                    && m_counter != nullptr;
            }
        };

        [[nodiscard]] static RenderGraphBuffer* CalcCounterPrefixSum( RenderGraph& renderGraph, const CalcConterPrefixSumParam& param );

        struct BuildWorkListParam
        {
            uint32 m_numDrawCallIds = 0;

            RenderGraphTexture* m_visibility = nullptr;

            RenderGraphBuffer* m_offset = nullptr;
            RenderGraphBuffer* m_indirectArgs = nullptr;

            bool IsValid() const
            {
                return m_numDrawCallIds > 0
                    && m_visibility != nullptr
                    && m_offset != nullptr
                    && m_indirectArgs != nullptr;
            }
        };

        [[nodiscard]] static RenderGraphBuffer* BuildWorkList( RenderGraph& renderGraph, const BuildWorkListParam& param );

        static void PrefixSumTestBed( RenderGraph& renderGraph );
    };
}
