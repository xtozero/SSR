#pragma once

#include "SparseArray.h"

#include <vector>

namespace rendercore
{
    class Material;
    class RenderGraph;

    struct TextureStreamingFeedback
    {
        std::vector<int32> m_minTextureSizes;
        uint64 m_frameIndex = 0;
        bool m_hasNewData = false;
    };

    class TextureStreamingManager
    {
    public:
        static TextureStreamingManager& GetInstance();

        void BootUp();
        void Shutdown();

        void Register( Material& material );
        void Unregister( int32 id );

        void Tick();

    private:
        void RequestStreamingFeedback();

        TextureStreamingManager() = default;

        SparseArray<Material*> m_materials;
        std::vector<TextureStreamingFeedback> m_feedbacks;

        std::vector<int32> m_pendingFeedbackIndices;
        std::vector<int32> m_freeFeedbackIndices;

        uint64 m_frameIndex = 0;
    };
}

namespace rendercore::renderthread
{
    struct TextureStreamingFeedbackContext
    {
        TextureStreamingFeedback* m_feedback = nullptr;
        uint64 m_frameIndex = 0;
        RefHandle<agl::Buffer> m_gpuFeedback;
    };

    class TextureStreamingManager
    {
    public:
        static TextureStreamingManager& GetInstance();

        void BootUp();
        void Shutdown();

        void EnqueueFeedback( TextureStreamingFeedback& feedback );

        agl::Buffer* AddFeedbackBufferClearPass( RenderGraph& renderGraph );
        void AddFeedbackBufferReadbackPass( RenderGraph& renderGraph, agl::Buffer* gpuFeedback );

        void Tick();

    private:
        TextureStreamingFeedbackContext* GetCurrentFeedbackContext();

        TextureStreamingManager() = default;

        std::vector<TextureStreamingFeedbackContext> m_feedbackQueue;
        std::atomic<uint64> m_frameIndex = 0;
    };
}