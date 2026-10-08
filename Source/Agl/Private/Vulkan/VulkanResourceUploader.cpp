#include "VulkanResourceUploader.h"

#include "VulkanApi.h"

namespace agl
{
    void VulkanUploadContext::Prepare()
    {
        m_cmdListResource = VulkanCmdPool( CommandListType::Copy ).GetCommandList();

        VkCommandBufferBeginInfo beginInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        };

        VkResult result = vkBeginCommandBuffer( m_cmdListResource.m_commandBuffer, &beginInfo );
        assert( result == VK_SUCCESS );
    }

    void VulkanUploadContext::RecordUploadCommand( VulkanBuffer& dest, const void* data, uint32 destOffset, uint32 numByte )
    {
        assert( m_destResource.Get() == nullptr );

        m_destResource = &dest;

        if ( numByte == 0 )
        {
            numByte = dest.Size();
        }

        BufferDesc desc = {
            .m_stride = numByte,
            .m_count = 1,
            .m_access = ResourceAccess::Upload,
            .m_bindType = ResourceBindType::None,
            .m_miscFlag = ResourceMisc::Intermediate,
            .m_format = ResourceFormat::Unknown
        };

        m_intermediateResource = Buffer::Create( desc, "Uploader.Buffer.Intermediate" );

        auto intermediate = RefStaticCast<VulkanBuffer>( m_intermediateResource );
        void* mappedData = intermediate->Lock().m_data;

        std::memcpy( mappedData, data, numByte );

        intermediate->UnLock();

        VkBufferCopy2 copyRegion = {
            .sType = VK_STRUCTURE_TYPE_BUFFER_COPY_2,
            .srcOffset = 0,
            .dstOffset = destOffset,
            .size = numByte,
        };

        VkCopyBufferInfo2 copyInfo = {
            .sType = VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2,
            .srcBuffer = intermediate->Resource(),
            .dstBuffer = dest.Resource(),
            .regionCount = 1,
            .pRegions = &copyRegion,
        };

        vkCmdCopyBuffer2( m_cmdListResource.m_commandBuffer, &copyInfo );

        vkEndCommandBuffer( m_cmdListResource.m_commandBuffer );
    }

    bool VulkanUploadContext::IsFinished() const
    {
        return vkGetFenceStatus( VulkanDevice(), m_cmdListResource.m_fence ) == VK_SUCCESS;
    }

    VkCommandBuffer VulkanUploadContext::CommandBuffer() const
    {
        return m_cmdListResource.m_commandBuffer;
    }

    VkFence VulkanUploadContext::Fence() const
    {
        return m_cmdListResource.m_fence;
    }

    void VulkanResourceUploader::Prepare()
    {
        VulkanUploadContext* uploadContext = m_pendingListForUpload;
        while ( uploadContext != nullptr )
        {
            if ( uploadContext->IsFinished() )
            {
                VulkanUploadContext* deleteTarget = uploadContext;
                uploadContext = uploadContext->m_next;

                SLinkedList::Remove( m_pendingListForUpload, deleteTarget );

                std::destroy_at( deleteTarget );
                m_uploadContextPool.Deallocate( deleteTarget );
            }
            else
            {
                uploadContext = uploadContext->m_next;
            }
        }
    }

    void VulkanResourceUploader::Upload( VulkanBuffer& dest, const void* data, uint32 destOffset, uint32 numByte )
    {
        VulkanUploadContext& context = *m_uploadContextPool.Allocate();
        std::construct_at( &context );

        SLinkedList::AddToHead( m_pendingListForUpload, &context );
        context.Prepare();
        context.RecordUploadCommand( dest, data, destOffset, numByte );

        VkCommandBufferSubmitInfo commandBufferSubmitInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
            .commandBuffer = context.CommandBuffer(),
            .deviceMask = 0,
        };

        VkSubmitInfo2 submitInfo = {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
            .commandBufferInfoCount = 1,
            .pCommandBufferInfos = &commandBufferSubmitInfo,
        };

        VkResult result = vkQueueSubmit2( VulkanTransferQueue(), 1, &submitInfo, context.Fence() );
        assert( result == VK_SUCCESS );
    }
}
