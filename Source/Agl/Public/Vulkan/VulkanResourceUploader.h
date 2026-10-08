#pragma once

#include "VulkanBuffer.h"
#include "VulkanCommandList.h"

namespace agl
{
    class VulkanResourceUploader;

    class VulkanUploadContext final
    {
    public:
        void Prepare();

        void RecordUploadCommand( VulkanBuffer& dest, const void* data, uint32 destOffset, uint32 numByte );

        bool IsFinished() const;

        VulkanUploadContext* m_next = nullptr;

    private:
        friend VulkanResourceUploader;

        VkCommandBuffer CommandBuffer() const;
        VkFence Fence() const;

        RefHandle<Buffer> m_intermediateResource;
        RefHandle<GraphicsApiResource> m_destResource;

        VulkanCommandListResource m_cmdListResource;
    };

    class VulkanResourceUploader final
    {
    public:
        void Prepare();

        void Upload( VulkanBuffer& dest, const void* data, uint32 destOffset = 0, uint32 numByte = 0 );

    private:
        FixedBlockMemoryPool<VulkanUploadContext> m_uploadContextPool;

        VulkanUploadContext* m_pendingListForUpload = nullptr;
    };


}
