#include "VulkanBarrierBatcher.h"

#include "GraphicsApiResource.h"
#include "VulkanFlagConverter.h"

namespace agl
{
    void VulkanBarrierBatcher::AddTransition( const ResourceTransition& transition )
    {
        ResourceState beforeState = transition.m_pTransitionable->GetResourceState();
        ResourceState afterState = transition.m_state;

        if ( beforeState == afterState )
        {
            return;
        }

        if ( transition.m_isBuffer )
        {
            VkBufferMemoryBarrier2 barrier = {
                .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
                .srcStageMask = ConvertToVkPipelineStageFlags2( beforeState ),
                .srcAccessMask = ConvertToVkAccessFlags2( beforeState ),
                .dstStageMask = ConvertToVkPipelineStageFlags2( afterState ),
                .dstAccessMask = ConvertToVkAccessFlags2( afterState ),
                .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .buffer = static_cast<VkBuffer>( transition.m_pResource ),
                .offset = 0,
                .size = VK_WHOLE_SIZE,
            };

            m_bufferMemoryBarriers.push_back( barrier );
        }
        else
        {
            VkImageMemoryBarrier2 barrier = {
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
                .srcStageMask = ConvertToVkPipelineStageFlags2( beforeState ),
                .srcAccessMask = ConvertToVkAccessFlags2( beforeState ),
                .dstStageMask = ConvertToVkPipelineStageFlags2( afterState ),
                .dstAccessMask = ConvertToVkAccessFlags2( afterState ),
                .oldLayout = ConvertToVkImageLayout( beforeState ),
                .newLayout = ConvertToVkImageLayout( afterState ),
                .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .image = static_cast<VkImage>( transition.m_pResource ),
                .subresourceRange = {
                    .aspectMask = ConvertToVkImageAspectFlags( afterState ),
                    .baseMipLevel = 0,
                    .levelCount = VK_REMAINING_MIP_LEVELS,
                    .baseArrayLayer = 0,
                    .layerCount = VK_REMAINING_ARRAY_LAYERS,
                },
            };

            m_imageMemoryBarriers.push_back( barrier );
        }

        transition.m_pTransitionable->SetResourceState( afterState );
    }

    void VulkanBarrierBatcher::AddUavBarrier( const UavBarrier& uavBarrier )
    {
    }

    void VulkanBarrierBatcher::Commit( VulkanCopyCommandListImpl& commandList )
    {
        if ( m_bufferMemoryBarriers.empty() && m_imageMemoryBarriers.empty() )
        {
            return;
        }

        auto numBufferBarriers = static_cast<uint32>( m_bufferMemoryBarriers.size() );
        auto numImageBarriers =  static_cast<uint32>( m_imageMemoryBarriers.size() );

        commandList.PipelineBarrier( numBufferBarriers, m_bufferMemoryBarriers.data(), numImageBarriers, m_imageMemoryBarriers.data() );

        m_bufferMemoryBarriers.clear();
        m_imageMemoryBarriers.clear();
    }

    VulkanBarrierBatcher::VulkanBarrierBatcher()
    {
        m_bufferMemoryBarriers.reserve( DefaultCapacity );
        m_imageMemoryBarriers.reserve( DefaultCapacity );
    }
}
