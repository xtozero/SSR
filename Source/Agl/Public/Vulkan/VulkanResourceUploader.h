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

        VulkanUploadContext* m_next = nullptr;

    private:
        friend VulkanResourceUploader;

        RefHandle<Buffer> m_intermediateResource;
        RefHandle<GraphicsApiResource> m_destResource;

        VulkanCommandListResource m_cmdListResource;
    };

    class VulkanResourceUploader final
    {
    public:
        void Upload( VulkanBuffer& dest, const void* data, uint32 dsetOffset = 0, uint32 numByte = 0 );

    private:
        FixedBlockMemoryPool<VulkanUploadContext> m_uploadContextPool;
    };


}
